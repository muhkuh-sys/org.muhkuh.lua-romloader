
%define lua_version 5.1
%define wxwidgets_version 2.8.0


Name:           muhkuh
Version:        1.0.0
Release:        1%{dist}
Summary:        A test tool for hardware designs
Group:          Productivity/Scientific/Electronics
License:        GPL
URL:            http://www.sf.net/projects/muhkuh
Source0:        file://tmp/muhkuh_%{version}/muhkuh-%{version}.tar.bz2
Patch0:         muhkuh-1.0.0-fwpatch.diff
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

BuildRequires:  gcc-c++ cmake
BuildRequires:  python >= 2.6
BuildRequires:  swig
BuildRequires:  lua-devel >= %{lua_version}
BuildRequires:  wxWidgets-devel >= %{wxwidgets_version}
BuildRequires:  libusb-devel = 0.1.12


%description
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

#----------------------------------------------------------------------------

%prep
%setup -q -n muhkuh-%{version}
%patch -P 0 -p 1

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

%package -n libmuhkuh_plugin_interface
Summary:        Common parts for the Muhkuh system
Group:          Productivity/Scientific/Electronics

%description -n libmuhkuh_plugin_interface
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

# The post action is executed after the installation. Run ldconfig to add the libraries to the systems list.
%post -n libmuhkuh_plugin_interface
/sbin/ldconfig

# The postun action is executed after the uninstallation. Run ldconfig to remove the libraries from the systems list.
%postun -n libmuhkuh_plugin_interface
/sbin/ldconfig

%files -n libmuhkuh_plugin_interface
%defattr(-,root,root)
%{_libdir}/libmuhkuh_plugin_interface.so

#----------------------------------------------------------------------------

%package lua-bit
Summary:        Bit operations for Lua
Group:          Productivity/Scientific/Electronics
Requires:       lua >= %{lua_version}

%description lua-bit
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

%files lua-bit
%defattr(-,root,root)
%{_libdir}/lua/5.1/bit.so

#----------------------------------------------------------------------------

%package lua-mhash
Summary:        MHash binding for Lua
Group:          Productivity/Scientific/Electronics
Requires:       lua >= %{lua_version}

%description lua-mhash
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

%files lua-mhash
%defattr(-,root,root)
%{_libdir}/lua/5.1/mhash.so

#----------------------------------------------------------------------------

%package lua
Summary:        The lua binding and scripts
Group:          Productivity/Scientific/Electronics
Requires:       lua >= %{lua_version}
Requires:       muhkuh-plugin_interface

%description lua
Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are
used to access the device under test or the testing equipment. Testcases are
written in Lua and can be easily extended by the plugins.

%files lua
%defattr(-,root,root)
%{_libdir}/lua/5.1/muhkuh.so
%{_libdir}/lua/5.1/romloader.so
%{_libdir}/lua/5.1/romloader_uart.so
%{_libdir}/lua/5.1/romloader_usb.so

#----------------------------------------------------------------------------

%changelog
