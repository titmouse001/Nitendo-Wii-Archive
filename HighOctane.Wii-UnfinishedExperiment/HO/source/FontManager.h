#ifndef FontManager_H_
#define FontManager_H_

class Font;

class FontManager
{

public:

	void LoadFont(const string FontName);
	Font* GetFont();

private:

	typedef vector<Font*>	TFontDetailsList;
	TFontDetailsList		m_vFontData;

};


#endif
