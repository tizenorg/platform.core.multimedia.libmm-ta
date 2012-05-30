Name:       libmm-ta
Summary:    Multimedia Framework Time Analysis Lib
Version:	0.1.4
Release:    1
Group:      System/Libraries
License:    Apache-2.0
Source0:    libmm-ta-%{version}.tar.gz
Source1001: packaging/libmm-ta.manifest 
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
cp %{SOURCE1001} .

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
/usr/lib/libmm_ta.so.*


%files devel
%manifest libmm-ta.manifest
/usr/include/mm_ta/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

