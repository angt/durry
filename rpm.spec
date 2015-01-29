%define moduledir %(pkg-config xorg-server --variable=moduledir )
%define driverdir %{moduledir}/drivers

Name:      xorg-x11-drv-durry
Version:   0.2
Release:   0%{?dist}
Summary:   Xorg X11 durry video driver
Group:     User Interface/X Hardware Support
License:   MIT
URL:       https://github.com/angt/durry
Source:    %{tarball}.tgz

ExcludeArch: s390 s390x

#BuildRequires: xorg-x11-server-sdk >= 1.4.99.1

Requires:  hwdata
Requires:  Xorg %(xserver-sdk-abi-requires ansic)
Requires:  Xorg %(xserver-sdk-abi-requires videodrv)

%description
Xorg X11 durry video driver

%prep
%setup -q -n %{tarball}

%build
autoreconf -i -f
%configure --disable-static
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

find $RPM_BUILD_ROOT -regex ".*\.la$" | xargs rm -f --

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{driverdir}/durry_drv.so
