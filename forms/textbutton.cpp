#include "library/sp.h"

#include "forms/textbutton.h"
#include "framework/framework.h"
#include "game/resources/gamecore.h"

namespace OpenApoc
{

TextButton::TextButton(UString Text, sp<BitmapFont> font)
    : Control(), text(Text), font(font), cached(nullptr),
      buttonclick(
          fw().data->load_sample("RAWSOUND:xcom3/RAWSOUND/STRATEGC/INTRFACE/BUTTON1.RAW:22050")),
      buttonbackground(fw().data->load_image("UI/TEXTBUTTONBACK.PNG")),
      TextHAlign(HorizontalAlignment::Centre), TextVAlign(VerticalAlignment::Centre),
      RenderStyle(TextButtonRenderStyles::MenuButtonStyle)
{
	if (font)
	{
		palette = font->getPalette();
	}
}

TextButton::~TextButton() {}

void TextButton::EventOccured(Event *e)
{
	Control::EventOccured(e);

	if (e->Type() == EVENT_FORM_INTERACTION && e->Forms().RaisedBy == shared_from_this() &&
	    e->Forms().EventFlag == FormEventType::MouseDown)
	{
		fw().soundBackend->playSample(buttonclick);
	}

	if (e->Type() == EVENT_FORM_INTERACTION && e->Forms().RaisedBy == shared_from_this() &&
	    e->Forms().EventFlag == FormEventType::MouseClick)
	{
		auto ce = new FormsEvent();
		ce->Forms() = e->Forms();
		ce->Forms().EventFlag = FormEventType::ButtonClick;
		fw().PushEvent(ce);
	}
}

void TextButton::OnRender()
{
	if (cached == nullptr || cached->size != Vec2<unsigned int>{Size.x, Size.y})
	{
		cached.reset(new Surface{Vec2<unsigned int>{Size.x, Size.y}});

		RendererSurfaceBinding b(*fw().renderer, cached);
		fw().renderer->clear();

		switch (RenderStyle)
		{
			case TextButtonRenderStyles::SolidButtonStyle:
				fw().renderer->drawFilledRect(Vec2<float>{0, 0}, Vec2<float>{Size.x, Size.y},
				                              BackgroundColour);
				break;
			case TextButtonRenderStyles::MenuButtonStyle:
				fw().renderer->drawScaled(buttonbackground, Vec2<float>{0, 0},
				                          Vec2<float>{Size.x, Size.y});
				fw().renderer->drawFilledRect(
				    Vec2<float>{3, 3}, Vec2<float>{Size.x - 6, Size.y - 6}, Colour{160, 160, 160});
				fw().renderer->drawLine(Vec2<float>{2, 4}, Vec2<float>{Size.x - 2, 4},
				                        Colour{220, 220, 220});
				fw().renderer->drawLine(Vec2<float>{2, Size.y - 4},
				                        Vec2<float>{Size.x - 2, Size.y - 4}, Colour{80, 80, 80});
				fw().renderer->drawLine(Vec2<float>{2, Size.y - 3},
				                        Vec2<float>{Size.x - 2, Size.y - 3}, Colour{64, 64, 64});
				fw().renderer->drawRect(Vec2<float>{3, 3}, Vec2<float>{Size.x - 6, Size.y - 6},
				                        Colour{48, 48, 48});
				break;
		}

		int xpos;
		int ypos;
		std::list<UString> lines = WordWrapText(font, text);

		switch (TextVAlign)
		{
			case VerticalAlignment::Top:
				ypos = 0;
				break;
			case VerticalAlignment::Centre:
				ypos = (Size.y / 2) - ((font->GetFontHeight() * lines.size()) / 2);
				break;
			case VerticalAlignment::Bottom:
				ypos = Size.y - (font->GetFontHeight() * lines.size());
				break;
			default:
				LogError("Unknown TextVAlign");
				return;
		}

		while (lines.size() > 0)
		{
			switch (TextHAlign)
			{
				case HorizontalAlignment::Left:
					xpos = 0;
					break;
				case HorizontalAlignment::Centre:
					xpos = (Size.x / 2) - (font->GetFontWidth(lines.front()) / 2);
					break;
				case HorizontalAlignment::Right:
					xpos = Size.x - font->GetFontWidth(lines.front());
					break;
				default:
					LogError("Unknown TextHAlign");
					return;
			}

			auto textImage = font->getString(lines.front());
			fw().renderer->draw(textImage, Vec2<float>{xpos, ypos});

			lines.pop_front();
			ypos += font->GetFontHeight();
		}
	}
	fw().renderer->draw(cached, Vec2<float>{0, 0});

	if (mouseDepressed && mouseInside)
	{
		switch (RenderStyle)
		{
			case TextButtonRenderStyles::SolidButtonStyle:
				fw().renderer->drawFilledRect(Vec2<float>{0, 0}, Vec2<float>{Size.x, Size.y},
				                              Colour{255, 255, 255});
				break;
			case TextButtonRenderStyles::MenuButtonStyle:
				fw().renderer->drawRect(Vec2<float>{1, 1}, Vec2<float>{Size.x - 2, Size.y - 2},
				                        Colour{255, 255, 255}, 2);
				break;
		}
	}
}

void TextButton::Update()
{
	// No "updates"
}

void TextButton::UnloadResources() {}

UString TextButton::GetText() const { return text; }

void TextButton::SetText(UString Text) { text = Text; }

sp<BitmapFont> TextButton::GetFont() const { return font; }

void TextButton::SetFont(sp<BitmapFont> NewFont) { font = NewFont; }

sp<Control> TextButton::CopyTo(sp<Control> CopyParent)
{
	sp<TextButton> copy;
	if (CopyParent)
	{
		copy = CopyParent->createChild<TextButton>(this->text, this->font);
	}
	else
	{
		copy = std::make_shared<TextButton>(this->text, this->font);
	}
	copy->TextHAlign = this->TextHAlign;
	copy->TextVAlign = this->TextVAlign;
	copy->RenderStyle = this->RenderStyle;
	CopyControlData(copy);
	return copy;
}

void TextButton::ConfigureFromXML(tinyxml2::XMLElement *Element)
{
	Control::ConfigureFromXML(Element);

	if (Element->Attribute("text") != nullptr)
	{
		text = tr(Element->Attribute("text"));
	}
	if (Element->FirstChildElement("font") != nullptr)
	{
		font = fw().gamecore->GetFont(Element->FirstChildElement("font")->GetText());
	}
}
}; // namespace OpenApoc
