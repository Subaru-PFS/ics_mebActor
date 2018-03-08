import distutils
from distutils.core import setup, Extension

import sdss3tools
import os

sdss3tools.setup(
    description = "MCS Electric Box",
    name = "ics_mebActor",
)

