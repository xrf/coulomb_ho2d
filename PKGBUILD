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
    s=`git 2>/dev/null describe --long --tags`
    if [ $? -eq 0 ]
    then
        printf '%s' "$s" | sed 's/^v//;s/\([^-]*-\)g/r\1/;s/-/./g'
    else
        printf 'r%s.%s' "`git rev-list --count HEAD`" \
                        "`git rev-parse --short HEAD`"
    fi
}

build() {
    cd "$srcdir/$pkgname"
    make
}

package() {
    cd "$srcdir/$pkgname"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 LICENSE.md "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
