#pragma once

#include "../MResource.h"
#include "../MStyleSheet.h"
#include <unordered_map>
#include <vector>
#include <string>
namespace MetalBone
{
	namespace CSS
	{
		// Do not change the enum value's order. Because the algorithm rely on the order.
		enum PropertyType {
			PT_InheritBackground,
			PT_Background,             PT_BackgroundClip,             PT_BackgroundRepeat,
			PT_BackgroundPosition,     PT_BackgroundSize,             PT_BackgroundAlignment,

			PT_BorderImage,

			PT_Border,                 PT_BorderWidth,                PT_BorderColor,
			PT_BorderStyles,
			PT_BorderTop,              PT_BorderRight,                PT_BorderBottom,
			PT_BorderLeft,
			PT_BorderTopWidth,         PT_BorderRightWidth,           PT_BorderBottomWidth,
			PT_BorderLeftWidth,
			PT_BorderTopColor,         PT_BorderRightColor,           PT_BorderBottomColor,
			PT_BorderLeftColor,
			PT_BorderTopStyle,         PT_BorderRightStyle,           PT_BorderBottomStyle,
			PT_BorderLeftStyle,
			PT_BorderTopLeftRadius,    PT_BorderTopRightRadius,       PT_BorderBottomLeftRadius,
			PT_BorderBottomRightRadius,
			PT_BorderRadius,

			PT_Margin,                 PT_MarginTop,                  PT_MarginRight,
			PT_MarginBottom,           PT_MarginLeft,
			PT_Padding,                PT_PaddingTop,                 PT_PaddingRight,
			PT_PaddingBottom,          PT_PaddingLeft,

			PT_Width,                  PT_Height,
			PT_MinimumWidth,           PT_MinimumHeight,
			PT_MaximumWidth,           PT_MaximumHeight,

			PT_Cursor,

			PT_Font,                   PT_FontSize,                   PT_FontStyle,
			PT_FontWeight,             PT_Color,
			PT_TextAlignment,          PT_TextDecoration,             PT_TextOverflow,
			PT_TextUnderlineStyle,     PT_TextOutline,                PT_TextShadow,

			knownPropertyCount
		};

		enum ValueType {
			Value_Unknown,    Value_None,        Value_Transparent,

			Value_Bold,
			Value_Normal,     Value_Italic,      Value_Oblique,
			Value_Clip,       Value_Ellipsis,    Value_Wrap,
			Value_Underline,  Value_Overline,    Value_LineThrough,

			Value_Dashed,     Value_DotDash,     Value_DotDotDash,
			Value_Dotted,     Value_Solid,       Value_Wave,

			Value_Padding,    Value_Border,      Value_Content,    Value_Margin,
			Value_Top,        Value_Right,       Value_Left,       Value_Bottom,    Value_Center,
			Value_NoRepeat,   Value_RepeatX,     Value_RepeatY,    Value_Repeat,    Value_Stretch,
			Value_True,

			// The Cursor's order must be the same as MCursor::CursorType
			Value_Default,   Value_AppStarting, Value_Cross,   Value_Hand,    Value_Help,    Value_IBeam,
			Value_Wait,      Value_Forbidden,   Value_UpArrow, Value_SizeAll, Value_SizeVer, Value_SizeHor,
			Value_SizeBDiag, Value_SizeFDiag,   Value_Blank,

			Value_SingleLoop,

			KnownValueCount
		};

		struct Selector;
		struct StyleRule;
		struct MatchedStyleRule
		{
			const Selector*  matchedSelector;
			const StyleRule* styleRule;
		};
		struct StyleSheet
		{
			typedef std::tr1::unordered_multimap<std::wstring, StyleRule*> StyleRuleElementMap;
			typedef std::tr1::unordered_multimap<std::wstring, StyleRule*> StyleRuleIdMap;
			~StyleSheet();
			std::vector<StyleRule*>	styleRules; // Keeps track of every StyleRule
			std::vector<StyleRule*>	universal;  // Universal
			StyleRuleElementMap		srElementMap;
			StyleRuleIdMap			srIdMap;
		};

		struct RenderRuleCacheKey
		{
			std::vector<StyleRule*> styleRules;
			bool operator<(const RenderRuleCacheKey& rhs) const;
			RenderRuleCacheKey& operator=(RenderRuleCacheKey& rhs) { styleRules = rhs.styleRules; }
		};

		struct CssValue
		{
			// Color store inside CssValue is ARGB format. (The same as MColor)
			enum  Type { Unknown, Number, Length, Identifier, Uri, Color, String };
			union Variant
			{
				int           vint;
				unsigned int  vuint;
				std::wstring* vstring;
				ValueType     videntifier;
			};

			Type    type;
			Variant data;

			inline CssValue(const CssValue&);
			explicit inline CssValue(Type t = Unknown);
			inline MColor getColor() const;
			inline const CssValue& operator=(const CssValue&);
		};
		typedef std::vector<CssValue> CssValueArray;

		// 1. StyleRule     - x:hover , y:clicked > z:checked { prop1: value1; prop2: value2; }
		// 2. Selector      - x:hover | y:clicked > z:checked
		// 3. BasicSelector - x:hover | y:clicked | z:checked
		// 4. Declaration   - prop1: value1; | prop2: value2;
		struct Declaration
		{
			~Declaration();
			void addValue(const std::wstring& css, int index, int length);
			PropertyType  property;
			CssValueArray values;
		};

		struct BasicSelector
		{
			enum Relation { NoRelation, MatchNextIfAncestor, MatchNextIfParent };

			BasicSelector():pseudo(PC_Default), pseudoCount(0), relationToNext(MatchNextIfAncestor){} 

			void addPseudoAndClearInput(std::wstring& p);

			std::wstring elementName; // ClassA .ClassA
			std::wstring id; // #Id
			unsigned int pseudo; // pseudo may contain a pseudo-element and many pseudo-classes
			unsigned int pseudoCount;
			Relation     relationToNext;
		};

		struct Selector
		{
			Selector(const std::wstring* css, int index = 0,int length = -1);
			~Selector();

			std::vector<BasicSelector*> basicSelectors;
			int  specificity() const;
			bool matchPseudo(unsigned int) const;
			inline unsigned int pseudo() const;
		};

		struct StyleRule
		{
			~StyleRule();
			std::vector<Selector*>    selectors;
			std::vector<Declaration*> declarations;
			unsigned int order;
		};

		class MCSSParser
		{
			public:
				MCSSParser(const std::wstring& c);
				void parse(StyleSheet*);

			private:
				const std::wstring* css;
				const int cssLength;
				int  pos;
				bool noSelector;

				void skipWhiteSpace();
				void skipComment();
				bool parseSelector(StyleRule*);
				void parseDeclaration(StyleRule*);

				MCSSParser(const MCSSParser&);
				const MCSSParser& operator=(const MCSSParser&);
		};

		struct CSSValuePair { const wchar_t* name; unsigned int value; };

		inline CssValue::CssValue(const CssValue& rhs):type(rhs.type),data(rhs.data){}
		inline CssValue::CssValue(Type t):type(t)
			{ data.vint = 0; }
		inline const CssValue& CssValue::operator=(const CssValue& rhs)
			{ type = rhs.type; data = rhs.data; return *this; }
		inline MColor CssValue::getColor() const
			{ M_ASSERT(type == Color); return MColor(data.vuint); }

		inline unsigned int Selector::pseudo() const
			{ return basicSelectors.at(basicSelectors.size() - 1)->pseudo; }
	} // namespace CSS
}