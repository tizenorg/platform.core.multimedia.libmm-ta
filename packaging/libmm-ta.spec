#sbs-git:slp/pkgs/l/libmm-ta libmm-ta 0.1.4 1859a89ab9fa666d844abd894b10584e408604f5

Name:       libmm-ta
Summary:    Multimedia Framework Time Analysis Lib
Version: 0.1.4
Release:    2
Group:      System/Libraries
License:    Apache-2.0
Source0:    libmm-ta-%{version}.tar.gz


%description
Multimedia Framework Time Analysis Library



%package devel
Summary:    Dev. components for the libmm-ta package (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development components for the libmm-ta package (devel)


%prep
%setup -q 


%build

%autogen --disable-static
%configure --disable-static
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install




%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig





%files
%manifest libmm-ta.manifest
%defattr(-,root,root,-)
%_libdir/libmm_ta.so.*


%files devel
%defattr(-,root,root,-)
%_includedir/mm_ta/*.h
%_libdir/*.so
%_libdir/pkgconfig/*.pc

