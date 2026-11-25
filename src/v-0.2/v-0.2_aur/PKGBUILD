# Maintainer: ltwtd <ltwtd.dev@yandex.ru>
pkgname=wectl-git
pkgver=0.2.r1.g0000000
pkgrel=1
pkgdesc="Ultra-lightweight pure C++20 modular Wallpaper Engine gallery daemon and TUI/CLI for Hyprland"
arch=('x86_64')
url="https://github.com/ltwtd/wectl"
license=('MIT')
depends=(
    'glibc'
    'gcc-libs'
    'hyprland'
    'procps-ng'
    'psmisc'
    'xdg-utils'
)
optdepends=(
    'linux-wallpaperengine-git: backend required for animated Steam Workshop scenes'
)
makedepends=(
    'git'
    'gcc'
    'make'
)
provides=('wectl')
conflicts=('wectl')
source=("${pkgname}::git+https://github.com/ltwtd/wectl.git")
sha256sums=('f6240f13b2a49080f671c2eb34436395dc8bf6ee7b4bfab549a4df21b33f66d0')

pkgver() {
    cd "$srcdir/$pkgname"
    git describe --long --tags --abbrev=7 2>/dev/null | sed 's/^v-//;s/^v//;s/\([^-]*-g\)/r\1/;s/-/./g' ||
    printf "0.1.r%s.g%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

build() {
    cd "$srcdir/$pkgname/src/v-.0.1_aur"
    CXXFLAGS="${CXXFLAGS} -O3 -flto" make build
}

package() {
    cd "$srcdir/$pkgname/src/v-.0.1_aur"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    if [ -f "$srcdir/$pkgname/LICENSE" ]; then
        install -Dm644 "$srcdir/$pkgname/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
    fi
}
