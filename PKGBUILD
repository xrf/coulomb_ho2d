# -*- shell-script -*-
# Maintainer: Fei Yuan <yuan@nscl.msu.edu>
pkgname=coulombho2d-git
pkgver=latest
pkgrel=1
pkgdesc="Coulomb matrix elements in 2D harmonic oscillator basis"
arch=(i686 x86_64)
url="https://github.com/xrf/coulomb_ho2d"
license=(MIT)
groups=()
depends=(glibc lapack)
makedepends=(git)
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=($pkgname::git://github.com/xrf/coulomb_ho2d)
noextract=()
sha256sums=(SKIP)

pkgver() {
    cd "$srcdir/$pkgname"
    git describe --long --tags | sed s/^v// | sed -r 's/([^-]*-g)/r\1/;s/-/./g'
}

build() {
    cd "$srcdir/$pkgname"
    make
}

package() {
    cd "$srcdir/$pkgname"
    make DESTDIR="$pkgdir/usr" install
    install -Dm644 LICENSE.md "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
