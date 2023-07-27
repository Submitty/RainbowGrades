#!/usr/bin/env python3

import os
import json
import subprocess

CURRENT_PATH = os.path.dirname(os.path.realpath(__file__))
RG_DIR = os.environ.get('RAINBOW_GRADES_DIRECTORY')
REPORT_DIR = os.environ.get('REPORTS_DIRECTORY')
RESULT_PATH = os.path.join(REPORT_DIR, "..", "rainbow_grades")

if __name__ == "__main__":

  json_dir = os.path.join(RESULT_PATH, "RG_version.json")
  output_dict = {}
  current_commit_hash_rg = 'unknown'
  current_short_commit_hash_rg = 'unknown'
  current_git_tag_rg = 'unknown'

  try:
    #run the command 'git rev-parse HEAD' from the RainbowGrades repository directory
    current_commit_hash_rg = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=RG_DIR)
    current_commit_hash_rg = current_commit_hash_rg.decode('ascii').strip()
    current_short_commit_hash_rg = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], cwd=RG_DIR)
    current_short_commit_hash_rg = current_short_commit_hash_rg.decode('ascii').strip()
    print("Commit {0} is currently installed on this system.".format(current_commit_hash_rg))
  except:
    print("ERROR: could not determine commit hash.")
    current_commit_hash_rg = 'unknown'

  try:
    #run the command 'git describe --tag --abbrev=0' from the RainbowGrades repository directory
    current_git_tag_rg = subprocess.check_output(['git', 'describe', '--tag', '--abbrev=0'], cwd=RG_DIR)
    current_git_tag_rg = current_git_tag_rg.decode('ascii').strip()
    print("Tag {0} is the most recent git tag.".format(current_git_tag_rg))
  except:
    print("ERROR: could not determine current git tag.")
    current_git_tag_rg = 'unknown'


  #remove newline at the end of the hash and tag and convert them from bytes to ascii.

  output_dict["installed_commit_rg"] = current_commit_hash_rg
  output_dict["short_installed_commit_rg"] = current_short_commit_hash_rg
  output_dict["most_recent_git_tag_rg"] = current_git_tag_rg


  try:
    #Update rainbow_grades/RG_version.json to reflect the current commit hash.
    with open(json_dir, 'w') as outfile:
       json.dump(output_dict, outfile, indent=2)
  except:
    print("ERROR: could not write to {0}".format(json_dir))
