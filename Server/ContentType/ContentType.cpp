#include "ContentType.hpp"

/*
** ContentType default constructor
*/
ContentType::ContentType(std::string const & f) : file(f) {
	content_type = "";
}

/*
** get content-type
*/
std::string const & ContentType::getContentType() {
	set_content_type();
	return content_type;
}

/*
** set content type
*/
void ContentType::set_content_type() {
	std::string temp;
	size_t index;

	index = file.find(".");
	if (index == std::string::npos) {
		content_type = "text/plain";
		return ;
	}
	temp = file.substr(index + 1);

	if (temp == "dwg")
		content_type = "application/acad";
	else if (temp == "ccad")
		content_type = "application/clariscad";
	else if (temp == "dxf")
		content_type = "application/dxf";
	else if (temp == "mdb")
		content_type = "application/msaccess";
	else if (temp == "doc")
		content_type = "application/msword";
	else if (temp == "bin")
		content_type = "application/octet-stream";
	else if (temp == "pdf")
		content_type = "application/pdf";
	else if (temp == "ai" || temp == "ps" || temp == "eps")
		content_type = "application/postscript";
	else if (temp == "rtf" || temp == "rtf")
		content_type = "application/rtf";
	else if (temp == "xls")
		content_type = "application/vnd.ms-excel";
	else if (temp == "ppt")
		content_type = "application/vnd.ms-powerpoint";
	else if (temp == "cdf")
		content_type = "application/x-cdf";
	else if (temp == "csh")
		content_type = "application/x-csh";
	else if (temp == "dvi")
		content_type = "application/x-dvi";
	else if (temp == "js")
		content_type = "application/x-javascript";
	else if (temp == "latex")
		content_type = "application/x-latex";
	else if (temp == "mif")
		content_type = "application/x-mif";
	else if (temp == "xls")
		content_type = "application/x-msexcel";
	else if (temp == "ppt")
		content_type = "application/x-mspowerpoint";
	else if (temp == "tcl")
		content_type = "application/x-tcl";
	else if (temp == "tex")
		content_type = "application/x-tex";
	else if (temp == "texinfo" || temp == "texi")
		content_type = "application/x-texinfo";
	else if (temp == "t" || temp == "tr" || temp == "roff t" || temp == "tr" || temp == "roff")
		content_type = "application/x-troff";
	else if (temp == "man")
		content_type = "application/x-troff-man";
	else if (temp == "me")
		content_type = "application/x-troff-me";
	else if (temp == "ms")
		content_type = "application/x-troff-ms";
	else if (temp == "src")
		content_type = "application/x-wais-source";
	else if (temp == "zip")
		content_type = "application/zip";
	else if (temp == "au" || temp == "snd")
		content_type = "audio/basic";
	else if (temp == "aif" || temp == "aiff" || temp == "aifc")
		content_type = "audio/x-aiff";
	else if (temp == "wav")
		content_type = "audio/x-wav";
	else if (temp == "gif")
		content_type = "image/gif";
	else if (temp == "ief")
		content_type = "image/ief";
	else if (temp == "jpeg" || temp == "jpg" || temp == "jpe")
		content_type = "image/jpeg";
	else if (temp == "tiff" || temp == "tif")
		content_type = "image/tiff";
	else if (temp == "ras")
		content_type = "image/x-cmu-raster";
	else if (temp == "pnm")
		content_type = "image/x-portable-anymap";
	else if (temp == "pbm")
		content_type = "image/x-portable-bitmap";
	else if (temp == "pgm")
		content_type = "image/x-portable-graymap";
	else if (temp == "ppm")
		content_type = "image/x-portable-pixmap";
	else if (temp == "rgb")
		content_type = "image/x-rgb";
	else if (temp == "xbm")
		content_type = "image/x-xbitmap";
	else if (temp == "xpm")
		content_type = "image/x-xpixmap";
	else if (temp == "xwd")
		content_type = "image/x-xwindowdump";
	else if (temp == "gzip")
		content_type = "multipart/x-gzip";
	else if (temp == "zip")
		content_type = "multipart/x-zip";
	else if (temp == "css")
		content_type = "text/css";
	else if (temp == "html" || temp == "htm")
		content_type = "text/html";
	else if (temp == "txt")
		content_type = "text/plain";
	else if (temp == "rtx")
		content_type = "text/richtext";
	else if (temp == "tsv")
		content_type = "text/tab-separated- values";
	else if (temp == "xml")
		content_type = "text/xml";
	else if (temp == "etx")
		content_type = "text/x-setext";
	else if (temp == "xsl")
		content_type = "text/xsl";
	else if (temp == "mpeg" || temp == "mpg" || temp == "mpe")
		content_type = "video/mpeg";
	else if (temp == "qt" || temp == "mov")
		content_type = "video/quicktime";
	else if (temp == "avi")
		content_type = "video/x-msvideo";
	else if (temp == "movie")
		content_type = "video/x-sgi-movie";
	else
		content_type = "text/plain";
}
