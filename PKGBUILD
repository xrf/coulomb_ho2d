# Maintainer: Fei Yuan <yuan@nscl.msu.edu>
pkgname=clh2-git
pkgver=latest
pkgrel=1
pkgdesc="Coulomb matrix elements in 2D harmonic oscillator basis"
arch=(i686 x86_64)
url=https://github.com/xrf/coulomb_ho2d
license=(MIT)
depends=(lapack)
makedepends=(git)
source=($pkgname::git://github.com/xrf/coulomb_ho2d)
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
    make all
}

package() {
    cd "$srcdir/$pkgname"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
