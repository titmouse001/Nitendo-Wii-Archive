#ifndef FontManager_H_
#define FontManager_H_

class Font;

class FontManager
{

public:

	FontManager()
	{
		M_FontColour.a = 0xff;
		M_FontColour.r = 0xff;
		M_FontColour.g = 0xff;
		M_FontColour.b = 0xff;
	}

	void LoadFont(const string& FontName);
	const Font* const GetFont() const;
	void SetFontColour(u8 Red,u8 Green,u8 Blue) { M_FontColour.r=Red; M_FontColour.g=Green, M_FontColour.b=Blue; M_FontColour.a= 0xff; }

	GXColor GetFontColour() const  { return M_FontColour; }

private:

	typedef vector<Font*>	TFontDetailsList;
	TFontDetailsList		m_vFontData;

	GXColor M_FontColour;
};


#endif
