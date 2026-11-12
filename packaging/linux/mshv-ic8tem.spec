%global debug_package %{nil}
%global __os_install_post %{nil}
Name:           mshv-ic8tem
Version:        %{mshv_ver}
Release:        %{mshv_rel}
Summary:        MSHV-IC8TEM - weak signal digital modes for amateur radio
License:        GPL-3.0-or-later
URL:            https://github.com/ic8tem/MSHV-IC8TEM

%description
Modified version of MSHV 2.76.7 by LZ2HV: Doppler tracking (CFOM) via CAT,
LoTW users marking and filter, fictitious log reset and more (IC8TEM).
User data are kept in ~/.local/share/MSHV-IC8TEM.

%install
cp -a %{stage}/. %{buildroot}/

%files
/opt/mshv-ic8tem
/usr/bin/mshv-ic8tem
/usr/share/applications/mshv-ic8tem.desktop
/usr/share/icons/hicolor/256x256/apps/mshv-ic8tem.png
%doc /usr/share/doc/mshv-ic8tem
