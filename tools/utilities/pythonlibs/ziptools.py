####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     ziptools.py
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import os
from os.path import basename
import zipfile

class Extractor:
    def __init__(self, archive):
        self.archive = archive

    def extract_file(self, file_extension, deleteExisting = True):
        """Extracts the first file matching the file extension"""
        _, ext = os.path.splitext(basename(self.archive))
        if (ext == ".zip"):
            with zipfile.ZipFile(self.archive) as zf:
                for member in zf.infolist():
                    _, e = os.path.splitext(member.filename)
                    if (e == file_extension):
                        path = os.path.dirname(self.archive)
                        if (deleteExisting and os.path.exists(path)):
                            os.remove(member.filename)
                        zf.extract(member, path)
                        return True, os.path.join(path, member.filename)
        else:
            return False, ""

class Zipper:
    def __init__(self):
        return

    def zip_file(self, input_file, output_zip):
        """Produces an output_zip with a single input_file compressed"""
        with zipfile.ZipFile(output_zip, "w", zipfile.ZIP_DEFLATED) as zf:
            zf.write(input_file, basename(input_file))
 