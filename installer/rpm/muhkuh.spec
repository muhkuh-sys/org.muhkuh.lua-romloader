Name:           muhkuh
Version:        1.0.0
Release:        1%{dist}
Summary:        A test tool for hardware designs
Group:          Applications/Engineering
License:        GPL
URL:            http://www.sf.net/projects/muhkuh
Source0:        file://tmp/muhkuh_%{version}/muhkuh-%{version}.tar.gz
#Source0:        http://downloads.sourceforge.net/project/muhkuh/muhkuh/muhkuh-%{version}/muhkuh-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  cmake
BuildRequires:  swig
Requires:       lua

%description
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

#----------------------------------------------------------------------------

%prep
%setup -q -n muhkuh-%{version}

%build
mkdir compile
cd compile
cmake -D CMAKE_INSTALL_PREFIX=/usr ..
make

%install
rm -rf %{buildroot}
cd compile
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

#----------------------------------------------------------------------------

%package common
Summary:        Common parts for the Muhkuh system
Group:          Applications/Engineering

%description common
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

# The post action is executed after the installation. Run ldconfig to add the libraries to the systems list.
%post common
/sbin/ldconfig

# The postun action is executed after the uninstallation. Run ldconfig to remove the libraries from the systems list.
%postun common
/sbin/ldconfig

%files common
%{_libdir}/libmuhkuh_plugin_interface.so

#----------------------------------------------------------------------------

%package lua
Summary:        The lua binding and scripts
Group:          Applications/Engineering

%description lua
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

%files lua
%{_libdir}/lua/5.1/bit.so
%{_libdir}/lua/5.1/mhash.so
%{_libdir}/lua/5.1/muhkuh.so
%{_libdir}/lua/5.1/romloader.so
%{_libdir}/lua/5.1/romloader_uart.so
%{_libdir}/lua/5.1/romloader_usb.so

#----------------------------------------------------------------------------

%changelog
