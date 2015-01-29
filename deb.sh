PKG_MAINTAINER="Adrien Gallouët <adrien@gallouet.fr>"
PKG_VERSION=${1:-1}
PKG_DIST=${2:-unstable}

PROG_NAME=xserver-xorg-video-durry
PROG_VERSION=0.2

rm -rf debian
mkdir debian
cd debian

cat >changelog<<EOF
${PROG_NAME} (${PROG_VERSION}-${PKG_VERSION}) ${PKG_DIST}; urgency=low

  * Initial release.

 -- ${PKG_MAINTAINER}  $(date -R)
EOF

cat >compat<<EOF
8
EOF

cat >control<<EOF
Source: ${PROG_NAME}
Section: x11
Priority: optional
Maintainer: ${PKG_MAINTAINER}
Build-Depends:
 debhelper (>= 8),
 dh-autoreconf,
 pkg-config,
 xserver-xorg-dev,
 x11proto-video-dev,
 x11proto-core-dev,
 xutils-dev

Package: ${PROG_NAME}
Architecture: any
Depends:
 \${shlibs:Depends},
 \${misc:Depends},
 \${xviddriver:Depends}
Provides:
 \${xviddriver:Provides}
Description: Xorg X11 durry video driver
EOF

cp ../LICENSE copyright

cat >${PROG_NAME}.install<<EOF
usr/lib/xorg/modules/drivers/*.so
EOF

cat >${PROG_NAME}.links<<EOF
usr/share/bug/xserver-xorg-core/script usr/share/bug/${PROG_NAME}/script
EOF

cat >rules<<EOF
#!/usr/bin/make -f

override_dh_auto_configure:
	dh_auto_configure -- --disable-silent-rules

override_dh_auto_install:
	dh_auto_install --destdir=debian/tmp

override_dh_install:
	find debian/tmp -name '*.la' -delete
	dh_install --fail-missing

override_dh_shlibdeps:
	dh_shlibdeps -- --warnings=6

%:
	dh \$@ --with autoreconf,xsf
EOF

chmod +x rules
