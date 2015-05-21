// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_ISTREAM_H)

#define __TT_ISTREAM_H

// new on 030703
class InputStream {
	public:
		InputStream() {
		};
		virtual ~InputStream() {
			// nothing here -- new on 150404
		};
		virtual byte get() = 0;
		virtual byte peek() = 0;
		virtual void read(char *bytes, int count) = 0; // was unsigned char * prior to 210703
		virtual void open(string file_name, int flags=0) = 0;
		virtual void close() = 0;
		virtual void seekg(int location) = 0;
		virtual string read_string(string s=NULL) = 0;
		virtual string read_string_old(string s=NULL) = 0; // need to load in old format cities with Marty in them
		virtual int tellg() = 0;
		virtual boolean empty() = 0;
		virtual void ignore(int count) {
			for (int i = 0; i < count; i++) {
				get();
			};
		};
};

class ZippedInputStream : public InputStream {
	public:
		ZippedInputStream() :
			InputStream() {
		};
		void open(string file_name, int flags=0) {
			archive = file_name;
		};
		byte get();
		byte peek();
		void read(char *bytes, int count);
		void close();
		int tellg();
		void seekg(int location);
		boolean empty();
		string read_string(string s=NULL);
		string read_string_old(string s=NULL) {
			read_string(s); // no different
		};
	protected:
		string archive;
};

class FileInputStream : public InputStream {
	public:
		FileInputStream() :
			InputStream() {
		};
		~FileInputStream() {
			close(); // new on 150404
		};
		byte get() {
			return(stream.get());
		};
		byte peek() {
			return(stream.peek());
		};
		void read(char *bytes, int count) {
			stream.read(bytes,count);
		};
		void open(string file_name, int flags=0) {
			stream.open(file_name,flags);
		};
		void close() {
			stream.close();
		};
		void seekg(int location) {
			stream.seekg(location);
		};
		int tellg() {
			return(stream.tellg());
		};
		boolean empty() {
			return(stream == NULL);
		};
		void ignore(int count) {
			stream.ignore(count);
		};
		string read_string(string s=NULL);
		string read_string_old(string s=NULL);
	protected:
		input_file_stream stream;
};

class StringInputStream : public InputStream {
	public:
		StringInputStream(string buffer, int length) :
			InputStream() {
				stream = new input_string_stream(buffer,length);
		};
		~StringInputStream() {
			delete stream;
		};
		byte get() {
			return(stream->get());
		};
		byte peek() {
			return(stream->peek());
		};
		void read(char *bytes, int count) {
			stream->read(bytes,count);
		};
		void open(string file_name, int flags=0) {
		};
		void close() {
		};
		void seekg(int location) {
			stream->seekg(location);
		};
		int tellg() {
			return(stream->tellg());
		};
		boolean empty() {
			return(stream == NULL);
		};
		void ignore(int count) {
			stream->ignore(count);
		};
		string read_string(string s=NULL);
		string read_string_old(string s=NULL);
	protected:
		input_string_stream *stream;
};

#endif