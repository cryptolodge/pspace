#ifndef _SUBSPACECHATBUFFER_H_
#define _SUBSPACECHATBUFFER_H_

#include <list>

using std::list;

#include "TextureFont.h"

//TODO: eventually make this stream-compatible
//TODO: dont use this thing

class SubspaceChatBuffer
{
private:
	struct Chunk
	{
		Chunk() : colorIndex(0)
		{}

		Chunk(const string& s, Uchar c) : text(s), colorIndex(c), headerColor(0)
		{}

		Chunk(const string& s, Uchar c, const string& h, Uchar hc) : text(s), colorIndex(c), header(h), headerColor(hc)
		{}

		bool equalState(const Chunk& rhs)
		{
			bool retval = true;
			if(colorIndex != rhs.colorIndex)
				retval = false;
			else if(header != rhs.header)
				retval = false;
			else if(headerColor != rhs.headerColor)
				retval = false;
			return retval;
		}

		string text;
		Uchar colorIndex;
		string header;
		Uchar headerColor;
	};

	typedef list<Chunk> ChunkList;
	typedef vector<ChunkList> ChunkLines;

public:
	
	SubspaceChatBuffer();
	~SubspaceChatBuffer();

	void setFont(const TextureFont& font);
	void setHeader(const string& header, Uchar colorIndex);	//for current chunk
	void setHeaderWidth(Uint width);
	void setLineWidth(Uint width);	//line width in characters - implied word wrap
	void setLinesDisplayed(Uint offset, Uint lines);
	
	//stream functions
	void flush();		//actually caches display
	void put(char c);
	void write(const char* s, int size);
	void write(const string& s);

	void addMessage(const string& text, Uchar colorIndex);
	void addMessage(const string& text, Uchar textColor, const string& header, Uchar headerColor);

	//Game functionality

	void display() const;
	void update(double time);

private:

	void addChunk(const Chunk& c);
	void cache();					//organizes entire chat buffer for display
	void cacheChunk(const Chunk& c);
	void cacheHeader(const Chunk& c);
	void cacheNewline(const Chunk* chunkOptions);
	void cacheWord(const Chunk& c);

private:

	TextureFont font_;
	Uint headerWidth_;
	Uint lineWidth_;
	Uint linesDisplayed_;
	Uint linesDisplayOffset_;

	ChunkList chunks_;
	Chunk currentChunk_;

	static const string whitespace;
	ChunkLines cachedLines_;	//for display
	ChunkList* currentLine_;
	Uint currentWidth_;
	bool rebuildCache_;

};

#endif