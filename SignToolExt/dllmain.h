// dllmain.h : ģ�����������

class CSignToolExtModule : public ATL::CAtlDllModuleT< CSignToolExtModule >
{
public :
	DECLARE_LIBID(LIBID_SignToolExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SIGNTOOLEXT, "{CC9C776E-1B6B-48FD-87C7-32FEF3433291}")
};

extern class CSignToolExtModule _AtlModule;
