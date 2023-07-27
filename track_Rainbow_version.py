#!/usr/bin/env python3

import os
import json
import subprocess

CURRENT_PATH = os.path.dirname(os.path.realpath(__file__))
RG_DIR = os.environ.get('RAINBOW_GRADES_DIRECTORY')


if __name__ == "__main__":

  json_dir = os.path.join(CURRENT_PATH, "RG_version.json")
  output_dict = {}
  current_commit_hash_rb = 'unknown'
  current_short_commit_hash_rb = 'unknown'
  current_git_tag_rb = 'unknown'
  print(RG_DIR)

  try:
    #run the command 'git rev-parse HEAD' from the submitty repository directory
    current_commit_hash_rb = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=RG_DIR)
    current_commit_hash_rb = current_commit_hash_rb.decode('ascii').strip()
    current_short_commit_hash_rb = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], cwd=RG_DIR)
    current_short_commit_hash_rb = current_short_commit_hash_rb.decode('ascii').strip()
    print("Commit {0} is currently installed on this system.".format(current_commit_hash_rb))
  except:
    print("ERROR: could not determine commit hash.")
    current_commit_hash_rb = 'unknown'

  try:
    #run the command 'git describe --tag --abbrev=0' from the submitty repository directory
    current_git_tag_rb = subprocess.check_output(['git', 'describe', '--tag', '--abbrev=0'], cwd=RG_DIR)
    current_git_tag_rb     = current_git_tag_rb.decode('ascii').strip()
    print("Tag {0} is the most recent git tag.".format(current_git_tag_rb))
  except:
    print("ERROR: could not determine current git tag.")
    current_git_tag = 'unknown'


  #remove newline at the end of the hash and tag and convert them from bytes to ascii.

  output_dict["installed_commit_rb"] = current_commit_hash_rb
  output_dict["short_installed_commit_rb"] = current_short_commit_hash_rb
  output_dict["most_recent_git_tag_rb"] = current_git_tag_rb


  try:
    #Update config/rainbow_version.json to reflect the current commit hash.
    with open(json_dir, 'w') as outfile:
       json.dump(output_dict, outfile, indent=2)
  except:
    print("ERROR: could not write to {0}".format(json_dir))
