PKG_NAME=durry
PKG_VERSION=0.2

PKG_DIR=${PKG_NAME}-${PKG_VERSION}
PKG_FILE=${PKG_DIR}.tar.gz

tar --transform "s/^\./${PKG_DIR}/" --exclude ${PKG_FILE} -zcf ${PKG_FILE} .
PKG_MD5=$(md5sum ${PKG_FILE} | cut -f1 -d' ')

cat > PKGBUILD <<EOF
pkgname=${PKG_NAME}
pkgver=${PKG_VERSION}
pkgrel=1
pkgdesc=""
arch=('i686' 'x86_64')
url=""
license=('MIT')
groups=()
depends=()
makedepends=()
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=('${PKG_FILE}')
md5sums=('${PKG_MD5}')
validpgpkeys=()

build() {
	cd "${PKG_DIR}"
	autoreconf -i -f
	./configure --prefix=/usr
	make
}

package() {
	cd "${PKG_DIR}"
	make DESTDIR="\$pkgdir/" install
}
EOF
