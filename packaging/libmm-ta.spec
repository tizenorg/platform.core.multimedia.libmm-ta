Name:       libmm-ta
Summary:    Multimedia Framework Time Analysis Lib
Version:	0.1.4
Release:    1
Group:      System/Libraries
License:    Apache-2.0
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
%autogen 
%configure 
make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
/usr/lib/libmm_ta.so.*


%files devel
/usr/include/mm_ta/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

