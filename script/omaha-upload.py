#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
import argparse
import logging
import os
import re
import sys

from argparse import RawTextHelpFormatter
from lib.config import get_brave_version, get_raw_version, get_env_var
from lib.helpers import *
from lib.connect import post, get, post_with_file
from lib.util import get_host_arch, omaha_channel
from lib.omaha import get_app_info, get_base64_authorization, get_channel_id, get_upload_version, get_event_id, \
                      get_channel_ids_from_omaha_server

# FIXME TODO:
# 1. write tests

# API DOCUMENTATION
# created id jenkins-upload account(PW IN 1password) on the `updates-panel-dev` omaha server
#   to generate base64 string on commandline use 'echo -n userid:password" | base64'
#### THE jenkins-upload USER CAN ONLY ADD OMAHA OR SPARKLE VERSION, CANNOT DELETE OR MODIFY EXISTING VERSIONS
# EXAMPLE CURL
# curl -L -D- -X GET -H "Authorization: Basic BASE64USERIDANDPASSWORD" -H "Content-Type: application/json" "http://OMAHA-HOSTNAME/api/omaha/version"
# https://crystalnix.github.io/omaha-server/#header-supplying-basic-auth-headers
# https://crystalnix.github.io/omaha-server/#omaha-version-version-list-post
# https://crystalnix.github.io/omaha-server/#sparkle-version-version-list-post

# Example of using API to communicate with Sparkle:
# https://gist.github.com/yurtaev/294a5fbd78016e5d7456

# TO-DO: Other apps need to be created 
def create_app(host, app_info, headers):
  url = 'http://' + host + '/api/app/'
  #print "app ids"
  #print get(url, headers)

  params = {
    "id": app_info['appguid'],
    "name": 'Brave-Browser-Darwin',
  }
  print post(url, params, headers)

def post_action(host, version, action, headers, args):
  url = 'http://' + host + '/api/action/'
  params = {
    "version": version,
    "event": get_event_id(action)
  }
  if release_channel() not in 'release':
    params['arguments'] = "--chrome-" + release_channel()

  if args.debug:
    logging.debug("params: ")
    for item in params:
        logging.debug('{}: {}'.format(item, params[item]))
  response = post(url, params, headers)

  if response.status_code != 201:
    logging.error("ERROR: Action not created! response.status_code : {}".format(response.status_code))
    exit(1)

def main():

  args = parse_args()

  if args.debug:
    logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
    logging.debug('brave_version: {}'.format(get_upload_version()))

  if args.file and args.github:
    exit("Error: --file and --github are mutually exclusive, only one allowed")

  if not os.environ.get('OMAHAPW') or not os.environ.get('OMAHAUSERID'):
    message = ('Error: Please set the $OMAHAUSERID, $OMAHAPW and $OMAHAHOST'
          'environment variables')
    exit(message)

  omahaid = os.environ.get('OMAHAUSERID')
  omahapw = os.environ.get('OMAHAPW')
  omahahost = os.environ.get('OMAHAHOST')

  if args.github:
    file_list = download_from_github(args)
  else:
    file_list = [ args.file ]

  for source_file in file_list:
    app_info = {}

    if args.debug:
      logging.debug("source_file: {}".format(source_file))

    if re.match(r'.*\.dmg$', source_file):
      app_info['platform'] = 'darwin'
      app_info['arch'] = 'x64'
    elif re.match(r'.*brave_installer-ia32\.exe$', source_file):
      app_info['platform'] = 'win32'
      app_info['arch'] = 'ia32'
    elif re.match(r'.*brave_installer-x64\.exe$', source_file):
      app_info['platform'] = 'win32'
      app_info['arch'] = 'x64'

    app_info = get_app_info(app_info, args)
    app_info['omahahost'] = omahahost
    app_info['auth'] = get_base64_authorization(omahaid, omahapw)
    app_info['headers'] = headers = {
                            'Authorization': 'Basic %s' % app_info['auth']
                          }

    if app_info['platform'] in 'darwin':
      app_info['version_url'] = '/api/sparkle/version/'
      if not os.environ.get('DSAPRIVPEM'):
        exit('Error: Please set the $DSAPRIVPEM environment variable')
    elif app_info['platform'] in 'win32':
      app_info['version_url'] = '/api/omaha/version/'

    app_info['version_post_url'] = 'http://' + app_info['omahahost'] + app_info['version_url']

    app_info['size'] = os.path.getsize(source_file)

    if args.debug:
      for item in app_info:
        logging.debug('{}: {}'.format(item, app_info[item]))
      logging.debug("omaha_channel: {}".format(omaha_channel(app_info['platform'], app_info['arch'], app_info['preview'])))
      logging.debug("omaha_channel_id: {}".format(get_channel_id(omaha_channel(app_info['platform'], app_info['arch'],
                                                  app_info['preview']), app_info['omahahost'], app_info['headers'], logging)))
      logging.debug("URL: {}".format(app_info['version_post_url']))
      logging.debug("file_list: {}".format(file_list))

    with open(source_file, 'rb') as f:
      files = {'file': f}
      params = {
        'app': app_info['appguid'],
        'channel': get_channel_id(omaha_channel(app_info['platform'], app_info['arch'], app_info['preview']),
                                  app_info['omahahost'], app_info['headers'], logging),
        'version': app_info['version'],
        'release_notes': app_info['release_notes']
      }
      if app_info['platform'] in 'win32':
        params['is_enabled'] = app_info['is_enabled']
        params['platform'] = app_info['platform_id']
      else:
        params['dsa_signature'] = app_info['darwindsasig']
        params['short_version'] = app_info['short_version']

      response = post_with_file(app_info['version_post_url'], files, params, headers)
      if response.status_code != 201:
        logging.error("ERROR: Version not created! response.status_code : {}".format(response.status_code))
        exit(1)

      # if downloading from github, remove files after upload
      if args.github:
        try:
          os.remove(source_file)
          logging.debug("Removed file: {}".format(source_file))
        except OSError:
          raise

    if app_info['platform'] in 'win32':
      # For windows add actions to version just created
      rjson = response.json()
      if args.debug:
        logging.debug("response['id']: {}".format(rjson['id']))
      post_action(app_info['omahahost'], rjson['id'], 'install', headers, args)
      post_action(app_info['omahahost'], rjson['id'], 'update', headers, args)

def download_from_github(args):
  from lib.githubdebug import GitHub

  file_list = []

  # BRAVE_REPO defined in helpers.py
  repo = GitHub(get_env_var('GITHUB_TOKEN')).repos(BRAVE_REPO)
  if args.tag:
    tag_name = args.tag
  else:
    tag_name = get_brave_version()
  release = None
  releases = get_releases_by_tag(repo, tag_name, include_drafts=True)
  if releases:
    if len(releases) > 1:
      exit("Error: More than 1 release exists with the tag: \'{}\'".format(tag_name))
    release = releases[0]
  for asset in release['assets']:
    if re.match(r'.*\.dmg$', asset['name']) \
                or re.match(r'brave_installer.*\.exe$', asset['name']):
      filename = asset['name']
      asset_id = asset['id']
      browser_download_url = asset['browser_download_url']
      asset_url = asset['url']
      print("direct_dl_url: {}".format(asset_url + '/' + filename))
      print("Getting: {}, ID: {}, browser_download_url: {}".format(filename, asset_id, browser_download_url))

      r = repo.releases(release['id']).assets.download(
          params={'id': asset_id, 'browser_download_url': browser_download_url, 'filename': filename},
          headers={'accept': 'application/octet-stream'}
      )
      if r.status_code == 200:
        file_list.append('./' + filename)
  return file_list

def parse_args():
  desc = "Upload Windows/Mac install files to Omaha server" \
         "\n\nRequires the following ENVIRONMENT VARIABLES be set:" \
         "\n\nCHANNEL: The Brave channel, i.e. \'release\', \'beta\', \'dev\'" \
         "\nOMAHAHOST: The FQDN hostname of the Omaha server to upload to. (without \'http:\\\\' prefix)" \
         "\nOMAHAUSERID: The UserID to use to login to the Omaha server." \
         "\nOMAHAPW: The Password to login to the Omaha server." \
         "\nDSAPRIVPEM: The Private DSA pem file used to sign the Mac DMG file." \
         "\nBRAVE_GITHUB_TOKEN: Github token to download from a draft release if not published yet. (ONLY REQUIRED IF --github)" \
         "\nnpm_config_brave_version: Chromium version (only if not in brave-core directory with brave-browser in the parent dir)"
  parser = argparse.ArgumentParser(description=desc, formatter_class=RawTextHelpFormatter)
  # parser.add_argument('-c', '--chrome-ver', help='Chrome version (if not in brave-core dir with brave-browser in the parent dir)')
  parser.add_argument('-d', '--debug', action='store_true', help='Print debug statements')
  parser.add_argument('-f', '--file', help='Windows or Mac install file to upload to' \
                      ' omaha/sparkle (cannot be combined with --github)')
  parser.add_argument('-g', '--github', action='store_true', help='Download Win and Mac install files' \
                      ' from Github before uploading to Omaha (cannot be combined with --file)')
  parser.add_argument('-p', '--preview', action='store_true', help='Preview channels for testing' \
                      ' omaha/sparkle uploads by QA before production release')
  parser.add_argument('-t', '--tag', help='Version tag to download from Github')
  return parser.parse_args()

if __name__ == '__main__':
  sys.exit(main())
