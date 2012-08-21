#sbs-git:slp/pkgs/l/libmm-ta libmm-ta 0.1.4 1859a89ab9fa666d844abd894b10584e408604f5

Name:       libmm-ta
Summary:    Multimedia Framework Time Analysis Lib
Version: 0.1.4
Release:    1
Group:      System/Libraries
License:    TO BE FILLED IN
Source0:    libmm-ta-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


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
%defattr(-,root,root,-)
/usr/lib/libmm_ta.so.*


%files devel
%defattr(-,root,root,-)
/usr/include/mm_ta/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

