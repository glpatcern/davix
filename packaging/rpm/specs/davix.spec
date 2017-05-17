# unversionned doc dir F20 change https://fedoraproject.org/wiki/Changes/UnversionedDocdirs
%{!?_pkgdocdir: %global _pkgdocdir %{_docdir}/%{name}-%{version}}

Name:				davix
Version:			0.6.7
Release:			1%{?dist}
Summary:			Toolkit for Http-based file management
Group:				Applications/Internet
License:			LGPLv2+
URL:				http://dmc.web.cern.ch/projects/davix/home
# git clone http://git.cern.ch/pub/davix
Source0:			http://grid-deployment.web.cern.ch/grid-deployment/dms/lcgutil/tar/%{name}/%{name}-%{version}.tar.gz
BuildRoot:			%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

#main lib dependencies
BuildRequires:                  python-virtualenv
BuildRequires:                  libuuid-devel
BuildRequires:                  cmake
BuildRequires:                  doxygen
BuildRequires:                  libxml2-devel
BuildRequires:                  openssl-devel

Requires: libuuid

# davix-copy dependencies
BuildRequires:                  gsoap-devel

# unit tests and abi check
%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 6
BuildRequires:                  abi-compliance-checker
%endif
BuildRequires:                  gtest-devel

Requires:                       %{name}-libs%{?_isa} = %{version}-%{release}



%description
Davix is a toolkit designed for file operations
with Http based protocols (WebDav, Amazon S3, ...).
Davix provides an API and a set of command line tools.

%package libs
Summary:			Development files for %{name}
Group:				Applications/Internet

%description libs
Libraries for %{name}. Davix is a toolkit designed for file operations
with Http based protocols (WebDav, Amazon S3, ...).


%package devel
Summary:			Development files for %{name}
Group:				Applications/Internet
Requires:			%{name}-libs%{?_isa} = %{version}-%{release}
Requires:			pkgconfig

%description devel
Development files for %{name}. Davix is a toolkit designed for file operations
with Http based protocols (WebDav, Amazon S3, ...).

%package doc
Summary:			Documentation for %{name}
Group:				Documentation
%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 6
BuildArch:	noarch
%endif

%description doc
Documentation and examples for %{name}. Davix is a toolkit designed
for file operations with Http based protocols (WebDav, Amazon S3, ...).

%clean
rm -rf %{buildroot}
make clean

%prep
%setup -q
# remove useless embedded components
rm -rf test/pywebdav/

%build
# setup a virtualenv with sphinx
%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 7
virtualenv /tmp/sphinx-venv
source /tmp/sphinx-venv/bin/activate

pip install --upgrade pip
pip install sphinx
%endif

%cmake \
-DDOC_INSTALL_DIR=%{_pkgdocdir} \
-DGTEST_EXTERNAL=TRUE \
-DENABLE_THIRD_PARTY_COPY=TRUE \
-DENABLE_HTML_DOCS=TRUE \
-DUNIT_TESTS=TRUE \
.
make %{?_smp_mflags}
make doc

%check
%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 6
make abi-check
%endif
ctest -V -T Test


%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%files
%defattr (-,root,root)
%{_bindir}/*
%{_mandir}/man1/*

%files libs
%defattr (-,root,root)
%{_libdir}/libdavix.so.*
%{_libdir}/libdavix_copy.so.*
%{_mandir}/man3/*


%files devel
%defattr (-,root,root)
%{_libdir}/libdavix.so
%{_libdir}/libdavix_copy.so
%dir %{_includedir}/davix
%{_includedir}/davix/*
%{_libdir}/pkgconfig/*

%files doc
%defattr (-,root,root)
%{_pkgdocdir}/LICENSE
%{_pkgdocdir}/RELEASE-NOTES
%{_pkgdocdir}/html/


%changelog
* Wed May 17 2017 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.7-1
 - davix 0.6.7 release, see RELEASE-NOTES for changes

* Thu May 11 2017 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.6-1
 - davix 0.6.6 release, see RELEASE-NOTES for changes

* Tue Feb 07 2017 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.5-1
 - davix 0.6.5 release, see RELEASE-NOTES for changes

* Thu Aug 18 2016 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.4-1
 - davix 0.6.4 release, see RELEASE-NOTES for changes

* Thu Apr 07 2016 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.3-1
 - davix 0.6.3 release, see RELEASE-NOTES for changes

* Tue Apr 05 2016 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.2-1
 - davix 0.6.2 release, see RELEASE-NOTES for changes

* Mon Apr 04 2016 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.1-1
 - davix 0.6.1 release, see RELEASE-NOTES for changes

* Thu Nov 05 2015 Georgios Bitzes <georgios.bitzes at cern.ch> - 0.6.0-1
 - davix 0.6.0 release, see RELEASE-NOTES for changes

* Fri Dec 05 2014 Adrien Devresse <adevress at cern.ch> - 0.4.0-1
 - davix 0.4.0 release, see RELEASE-NOTES for changes

* Tue Aug 12 2014 Adrien Devresse <adevress at cern.ch> - 0.3.6-1
 - davix 0.3.6 release, see RELEASE-NOTES for changes

* Tue Jul 22 2014 Adrien Devresse <adevress at cern.ch> - 0.3.4-1
 - Update to release 0.3.4

* Wed Jun 04 2014 Adrien Devresse <adevress at cern.ch> - 0.3.1-1
 - davix 0.3.1 release, see RELEASE-NOTES for changes

* Tue Jun 03 2014 Adrien Devresse <adevress at cern.ch> - 0.3.0-1
 - davix 0.3.0 release, see RELEASE-NOTES for changes

* Tue Jan 28 2014 Adrien Devresse <adevress at cern.ch> - 0.2.10-1
 - davix 0.2.10 release, see RELEASE-NOTES for details

* Mon Oct 28 2013 Adrien Devresse <adevress at cern.ch> - 0.2.7-3
 - New update of davix, see RELEASE-NOTES for details


* Tue Sep 03 2013 Adrien Devresse <adevress at cern.ch> - 0.2.6-1
 - Release 0.2.6 of davix, see RELEASE-NOTES for details


* Wed Jun 05 2013 Adrien Devresse <adevress at cern.ch> - 0.2.2-2
 - Initial EPEL release
