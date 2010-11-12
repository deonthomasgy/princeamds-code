#!/usr/bin/env python

from distutils.core import setup

setup(name = "smb-admin",
	version = "1.08.03",
	description = "This program was designed to assist users in managing samba settings, shares and users. It's purpose it to be simple yet allow the user plenty of options. For a more powerful samba admin program, look at gsamabd or SWAT.",
	author = "David Braker",
	author_email = "linuxnit@elivecd.org",
	packages=["smb-admin"],
	data_files=[("/usr/share/pixmaps",["src/smb-admin.png"]),
		    ("/usr/share/applications/",["src/smb-admin.desktop"]),
		    ("/usr/bin/",["src/smb-usershare"]),
		    ("/usr/sbin/",["src/smb-admin"])]
	)