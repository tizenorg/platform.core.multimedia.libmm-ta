Name:       libmm-ta
Summary:    Multimedia Framework Time Analysis Lib
Version:    0.1.4
Release:    0
Group:      System/Libraries
License:    Apache-2.0
Source0:    libmm-ta-%{version}.tar.gz
Source1001: libmm-ta.manifest

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
cp %{SOURCE1001} .

%build
%reconfigure --disable-static
%__make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libmm_ta.so.*

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/mm_ta/*.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
