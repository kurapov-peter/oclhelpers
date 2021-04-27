#!/usr/bin/env python3

import argparse
import datetime
import tempfile
from pathlib import Path
import os
import subprocess

project_root_path = os.path.dirname(os.path.realpath(__file__))
parser = argparse.ArgumentParser(description='Create release artifacts.')
parser.add_argument(
    'destination', help='Destination folder for the package.', type=Path)
parser.add_argument('version', help='Package version in x.x.x format.', type=str)

args = parser.parse_args()

package_path = Path(args.destination)
package_version = args.version

Path(package_path).mkdir(parents=True, exist_ok=True)

subprocess.run(["cmake", "--install", ".", "--prefix",  str(package_path)])


def get_artifact(config: str):
    path = Path(tmpdirname + "/" + config)
    path.mkdir()

    subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=" + config,
                    "-S", project_root_path, "-B", str(path)])
    subprocess.run(["cmake", "--build", str(path)])
    subprocess.run(["cmake", "--install", str(path),
                    "--prefix",  str(path) + "/install"])
    subprocess.run(["tar", "-czvf", str(package_path / ("oclhelpers-v" +
                                                        package_version + "-" + config + ".tar.gz")), "-C", str(path) + "/install", "."])


with tempfile.TemporaryDirectory() as tmpdirname:
    get_artifact("Debug")
    get_artifact("Release")
