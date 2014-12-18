
#include "stdafx.h"
#include "MarmaladeAutoUITexture.h"
#include "MarmaladePlugIn.h"

CMarmaladeAutoUITextureFactory::CMarmaladeAutoUITextureFactory()
{
}

CRhRdkTexture* CMarmaladeAutoUITextureFactory::NewTexture(void) const
{
	return new CMarmaladeAutoUITexture;
}

static const wchar_t* wszColor1 = L"color-1";
static const wchar_t* wszColor2 = L"color-2";

class CMarmaladeAutoUITexture::Evaluator : public CRhRdkTextureEvaluator
{
public:
	Evaluator(const CRhRdkColor& col1, const CRhRdkColor& col2, const ON_Xform& xform);

	virtual void DeleteThis(void) { delete this; }
	virtual bool GetColor(const ON_3dPoint& uvw, const ON_3dVector& duvwdx,
		                  const ON_3dVector& duvwdy, CRhRdkColor& colOut, void* pvData = NULL) const;
	virtual void* EVF(const wchar_t* wszFunc, void* pvData) { return NULL; }

private:
	CRhRdkColor m_color1;
	CRhRdkColor m_color2;
	ON_Xform m_xform;
};

CMarmaladeAutoUITexture::Evaluator::Evaluator(const CRhRdkColor& col1, const CRhRdkColor& col2, const ON_Xform& xform)
{
	m_color1 = col1;
	m_color2 = col2;
	m_xform  = xform;
}

bool CMarmaladeAutoUITexture::Evaluator::GetColor(const ON_3dPoint& uvw, const ON_3dVector& duvwdx,
                                                  const ON_3dVector& duvwdy, CRhRdkColor& colOut, void* pvData) const
{
	const ON_3dVector uvwNew = m_xform * (ON_Xform(2) * uvw);

	const int sum = (int)(uvwNew.x + 10000.0) + (int)(uvwNew.y + 10000.0);

	colOut = (sum % 2) ? m_color1 : m_color2;

	return true;
}

//---------------------

CMarmaladeAutoUITexture::CMarmaladeAutoUITexture()
{
	m_color1 = RGB(255, 140, 0);
	m_color2 = RGB(255, 255, 255);
}

CMarmaladeAutoUITexture::~CMarmaladeAutoUITexture()
{
}

UUID CMarmaladeAutoUITexture::RenderEngineId(void) const
{
	return CMarmaladePlugIn::ID();
}

UUID CMarmaladeAutoUITexture::PlugInId(void) const
{
	return CMarmaladePlugIn::ID();
}

UUID CMarmaladeAutoUITexture::TypeId(void) const
{
	//**********************************************************************
	// Generated for this class using GUIDGEN  *** Don't reuse this GUID ***
	// {5E33DC89-8217-4048-B1EF-F0EB4A8F03D2}
	static const GUID uuidTypeId =
	{
		0x5e33dc89, 0x8217, 0x4048, { 0xb1, 0xef, 0xf0, 0xeb, 0x4a, 0x8f, 0x03, 0xd2 }
	};
	//**********************************************************************

	return uuidTypeId;
}

ON_wString CMarmaladeAutoUITexture::TypeName(void) const
{
	return L"Marmalade Automatic UI Texture";
}

ON_wString CMarmaladeAutoUITexture::TypeDescription(void) const
{
	return L"Marmalade Automatic UI Texture";
}

ON_wString CMarmaladeAutoUITexture::InternalName(void) const
{
	return L"MarmaladeAutoUITexture";
}

void CMarmaladeAutoUITexture::SimulateTexture(CRhRdkSimulatedTexture& tex, bool bForDataOnly) const
{
	CRhRdkTexture::SimulateTexture(tex, bForDataOnly);
}

void CMarmaladeAutoUITexture::AddUISections(void)
{
	AddAutomaticUISection(L"Marmalade parameters");

	CRhRdkTexture::AddUISections();
}

DWORD CMarmaladeAutoUITexture::BitFlags(void) const
{
	return CRhRdkTexture::BitFlags() & ~bfTextureSummary // No texture summary required.
	                                 |  bfSharedUI;      // Shared UI supported.
}

void CMarmaladeAutoUITexture::AddAutoParameters(IRhRdkParamBlock& paramBlock, int iId)
{
	// It is through this method that the values in your content get transferred into the automatic UI.
	paramBlock.Add(wszColor1, L"", L"Color 1", m_color1.OnColor(), CRhRdkVariant::Null(), CRhRdkVariant::Null());
	paramBlock.Add(wszColor2, L"", L"Color 2", m_color2.OnColor(), CRhRdkVariant::Null(), CRhRdkVariant::Null());
}

void CMarmaladeAutoUITexture::GetAutoParameters(const IRhRdkParamBlock& paramBlock, int iId)
{
	// This method is called when something changes in the automatic UI as a result
	// of the user clicking on a control. It can be called from more than one UI section
	// even a section that doesn't contain our color parameters. This happens for example
	// with the Local Mapping section whose parameters are managed by the base class
	// CRhRdkTexture. Because of this, it is important to make sure that you don't
	// set your members if IRhRdkParamBlock::Get() returns false.

	CRhRdkVariant vValue;

	if (paramBlock.Get(wszColor1, vValue))
	{
		m_color1 = vValue.AsOnColor();
	}

	if (paramBlock.Get(wszColor2, vValue))
	{
		m_color2 = vValue.AsOnColor();
	}
}

bool CMarmaladeAutoUITexture::SetParameters(IRhRdk_XMLSection& section, eSetParamsContext context) const
{
	section.SetParam(wszColor1, m_color1.OnColor());
	section.SetParam(wszColor2, m_color2.OnColor());

	return __super::SetParameters(section, context);
}

bool CMarmaladeAutoUITexture::GetParameters(const IRhRdk_XMLSection& section, eGetParamsContext context)
{
	CRhRdkVariant v;

	if (section.GetParam(wszColor1, v))
		m_color1 = v.AsOnColor();

	if (section.GetParam(wszColor2, v))
		m_color2 = v.AsOnColor();

	return __super::GetParameters(section, context);
}

void* CMarmaladeAutoUITexture::GetShader(const UUID& uuidRenderEngine, void* pvData) const
{
	return NULL;
}

bool CMarmaladeAutoUITexture::IsFactoryProductAcceptableAsChild(const IRhRdkContentFactory* pFactory,
                                                                const wchar_t* wszChildSlotName) const
{
	const ON_wString sFactoryKind = pFactory->Kind();

	if (0 == sFactoryKind.CompareNoCase(RDK_KIND_TEXTURE))
		return true; // Factory produces textures.

	return false; // Factory produces something "unpalatable".
}

IRhRdkTextureEvaluator* CMarmaladeAutoUITexture::NewTextureEvaluator(void) const
{
	return new Evaluator(m_color1, m_color2, LocalMappingTransform());
}
