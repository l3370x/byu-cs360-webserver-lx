#include "handler.h"

Handler::Handler(Config c, bool sD) {
	cfg = c;
	showDebug = sD;
}
Handler::~Handler() {}





#define MAX_MSG_SZ      1024



// Determine if the character is whitespace
bool isWhitespace(char c)
{
	switch (c)
	{
	case '\r':
	case '\n':
	case ' ':
	case '\0':
		return true;
	default:
		return false;
	}
}

// Strip off whitespace characters from the end of the line
void chomp(char *line)
{
	int len = strlen(line);
	while (isWhitespace(line[len]))
	{
		line[len--] = '\0';
	}
}

// Read the line one character at a time, looking for the CR
// You dont want to read too far, or you will mess up the content
char * GetLine(int fds)
{
	char tline[MAX_MSG_SZ];
	char *line;

	int messagesize = 0;
	int amtread = 0;
	while((amtread = read(fds, tline + messagesize, 1)) < MAX_MSG_SZ)
	{
		if (amtread > 0)
			messagesize += amtread;
		else
		{
			perror("Socket Error is:");
			fprintf(stderr, "Read Failed on file descriptor %d messagesize = %d\n", fds, messagesize);
			exit(2);
		}
		//fprintf(stderr,"%d[%c]", messagesize,message[messagesize-1]);
		if (tline[messagesize - 1] == '\n')
			break;
	}
	tline[messagesize] = '\0';
	chomp(tline);
	line = (char *)malloc((strlen(tline) + 1) * sizeof(char));
	strcpy(line, tline);
	//fprintf(stderr, "GetLine: [%s]\n", line);
	return line;
}

// Change to upper case and replace with underlines for CGI scripts
string UpcaseAndReplaceDashWithUnderline(string s)
{

	//std::string s
	char *a=new char[s.size()+1];
	a[s.size()]=0;
	memcpy(a,s.c_str(),s.size());

	int i;
	char *ss;

	ss = a;
	for (i = 0; ss[i] != ':'; i++)
	{
		if (ss[i] >= 'a' && ss[i] <= 'z')
			ss[i] = 'A' + (ss[i] - 'a');

		if (ss[i] == '-')
			ss[i] = '_';
	}
	return a;
}


// When calling CGI scripts, you will have to convert header strings
// before inserting them into the environment.  This routine does most
// of the conversion
string FormatHeader(string str, char *prefix)
{
	string result;
	string value = str.substr(str.find_first_of(':',0) + 2,str.length());
	string next = UpcaseAndReplaceDashWithUnderline(str);
	next = next.substr(0,next.find_first_of(':',0));
	result = prefix + next + "=" + value;
	return result;
}


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	return split(s, delim, elems);
}

// Get the header lines from a socket
//   envformat = true when getting a request from a web client
//   envformat = false when getting lines from a CGI program

void GetHeaderLines(vector<string> &headerLines, HTTPRequest req, bool envformat)
{
	// Read the headers, look for specific ones that may change our responseCode
	char *line;
	char *tline;


	string heads = req.pstr();
	vector<string> headsV = split(heads,'\n');
	vector<string>::iterator it = headsV.begin();
	it = it + 1;

	for (; it != headsV.end() ; it++) {
		string theHead = FormatHeader((*it),"HTTP_");
		cout << "\tYour vector of headsV contains: " << theHead << endl;
	}

}


//************************************
//Handle input
//
//************************************
bool Handler::handle(int client, ClientData * cd) {
	this->cd = cd;

	// read a request
	memset(buf_,0,1501);
	int nread = recv(client,buf_,1500,0);
	if (nread < 0) {
		if (errno == EINTR)
			// the socket call was interrupted -- try again
			return true;
		// an error occurred so stop processing
		perror("recv");
		return false;
	} else if (nread == 0) {
		// the socket is closed
		return false;
	}

	string received = buf_;
	cd->recv(client,received);


	return checkBuffer(client);


}


//**********************************************
//CHECK A CLIENT'S BUFFER AND SEE IF THERE ARE
//ANY /R/N/R/N AND THEN PARSE MESSAGE IF SO
//
//**********************************************
bool Handler::checkBuffer(int client){
	//check for /r/n/r/n
	Client * c = cd->getClient(client);
	int found = 0;
	if(c != NULL){
		found = c->buffer.find("\r\n\r\n");
	}
	if(found != string::npos){
		return parseBuffer(client,found, c);
	}

	//no /r/n/r/n was found so wait for more
	return true;

}

//**********************************************
//PARSE A CLIENT'S BUFFER BECAUSE A /R/N/R/N WAS
//FOUND
//
//**********************************************
bool Handler::parseBuffer(int client,int found, Client *c){
	bool head = false;
	string received = c->buffer.substr(0,found+4);
	//clear the message off the client's buffer
	c->buffer = c->buffer.substr(found+4);

	//parse request
	HTTPRequest req;
	req.parse(received);

	if(req.uri().substr(req.uri().length()-3,req.uri().length()).compare("ico") == 0) {
		if(showDebug)
			cout << "HIDING ICO INFO" << endl;
		return false;
	}

	//create response
	HTTPResponse resp;
	resp.code("200");
	resp.phrase("OK");
	resp.header("Date",UsefulFunctions::getDate());
	resp.header("Server","l3370x");

	if(showDebug){
		cout<< "Buffer=\"" << received << "\"" << endl;
	}



	if(received.compare(0,3,"GET") != 0){
		if(received.compare(0,4,"HEAD") != 0){
			resp.code("501");
			resp.phrase("Not Implemented");
			return doResponse(resp,client);
		} else
			head = true;
	}

	//is a valid GET or HEAD request.
	//now get the file path requested
	string path;
	string host = req.header("Host");
	host = host.substr(0,host.find_first_of(":"));
	host = "default";
	path += cfg.host(host);
	if(cfg.host(host).length() == 0 || req.uri().length() == 0){
		resp.code("400");
		resp.phrase("Bad Request");
		return doResponse(resp,client);
	}
	if(req.uri().compare("/") == 0){
		path += "/index.html";
	} else {
		path += req.uri();
	}
	if(showDebug)
		cout << "the parsed path is :" << path << endl;


	//check file permissions
	int fdOut;
	fdOut = open(path.c_str(),O_RDONLY);
	if(fdOut < 0){
		if(errno == EACCES){
			resp.code("403");
			resp.phrase("Forbidden");
			return doResponse(resp,client);
		}
		if(errno == ENOENT){
			resp.code("404");
			resp.phrase("Not Found");
			return doResponse(resp,client);
		}
		resp.code("500");
		resp.phrase("Internal Server Error");
		perror("open");
		return doResponse(resp,client);
	}

	//check content length and last accessed
	struct stat attrib;
	stat(path.c_str(), &attrib);
	int contLength = attrib.st_size;
	bool isDir = false;
	string theDirs = "";
	if(attrib.st_mode & S_IFDIR != 0) {
		isDir = true;
		if(showDebug)
			cout << "\n\n!!!!!!!!!!!!DIRECTORY FOUND!!!!!!!!!!!!!!!!\n" << endl;
		DIR *dirp;
		struct dirent *dp;

		dirp = opendir(path.c_str());
		theDirs += "<html><body><h1>Index of localhost";
		theDirs += req.uri();
		theDirs += "</h1><hr><table><tr><td>Name</td><td>Size</td><td>Modified</td></tr>";

		while ((dp = readdir(dirp)) != NULL)
		{
			string name = dp->d_name;

			if (name == ".." || name == ".")
				continue;

			string pathD = path;
			pathD += "/";
			pathD += name;

			if(showDebug)
				cout << "File inside path to check is " << pathD << endl;


			struct stat attribD;
			stat(pathD.c_str(), &attribD);
			int contLengthD = attribD.st_size;
			if(attribD.st_mode & S_IFDIR != 0) {
				contLengthD = -1;
			}

			theDirs += "<tr>";
			theDirs += "<td><a href=\"" + req.uri() + "/" + name + "\">" + name + "</a></td><td>";
			if(contLengthD > 0)
				theDirs += UsefulFunctions::convertInt(contLengthD);
			theDirs += "</td><td>";
			theDirs += UsefulFunctions::date((attribD.st_mtime));
			theDirs += "</td></tr>";
		}
		theDirs += "</table><hr></body></html>";
	}

	if(head)
		contLength = contLength-1;
	string lastMod  = UsefulFunctions::date((attrib.st_mtime));


	//check content type
	string responseType;
	string webType = path;
	if(webType.length() > 0)
		webType = webType.substr(webType.find_last_of(".") + 1);
	responseType = cfg.media(webType);
	if(responseType.length() == 0)
		responseType = "text/plain";
	if(isDir)
		responseType = "text/html";
	if(showDebug){
		cout << "content length is: " << contLength << endl;
		cout << "last modification is: " << lastMod << endl;
		cout << "media type is: " << responseType << endl;
	}
	string CGIpath = "";
	if (webType.compare("cgi") == 0) {
		CGIpath = path;

		if(showDebug)
			cout << "\n\nCGI FOUND, path will be: " << CGIpath << endl;
	}

	int rangeReqStart = 0;
	int rangeReqEnd = 0;
	bool range = false;
	if(req.header("Range").length() > 0){
		range = true;
		string rng = req.header("Range");
		rng = rng.substr(5,string::npos);
		string myrangeReqStart = rng.substr(1,rng.find_first_of("-")-1);
		string myrangeReqEnd = rng.substr(rng.find_first_of("-")+1,string::npos);
		rangeReqStart = atoi(myrangeReqStart.c_str());
		rangeReqEnd = atoi(myrangeReqEnd.c_str());
		if(showDebug){
			cout << "range Start is " << rangeReqStart << endl;
			cout << "range End is " << rangeReqEnd << endl;
		}
	}


	if(range){
		return doResponse(resp,client,head,fdOut,contLength,lastMod,responseType,range,rangeReqStart,rangeReqEnd);
	}
	return doResponse(resp,client,head,fdOut,contLength,lastMod,responseType,false,0,0,isDir,theDirs, CGIpath, req);

}

//**********************************************
//DO A RESPONSE
//
//**********************************************
bool Handler::doResponse(HTTPResponse resp, int client,bool head, int fdOut,
		int len , string mod, string type,
		bool range, int start, int end, bool isDir, string dirs, string CGIpath, HTTPRequest req){
	if(showDebug) {
		cout << "\ndoing doResponse!" << endl;
	}
	const char *ptr;
	string rStr;
	if(resp.code().compare("400") == 0){
		resp.header("Content-Length", my400ERROR.length());
		rStr = resp.str();
		rStr+=my400ERROR;
	} else if(resp.code().compare("501") == 0){
		resp.header("Content-Length", my501ERROR.length());
		rStr = resp.str();
		rStr+=my501ERROR;
	} else if(resp.code().compare("500") == 0){
		resp.header("Content-Length", my500ERROR.length());
		rStr = resp.str();
		rStr+=my500ERROR;
	} else if(resp.code().compare("403") == 0){
		resp.header("Content-Length", my403ERROR.length());
		rStr = resp.str();
		rStr+=my403ERROR;
	} else if(resp.code().compare("404") == 0){
		resp.header("Content-Length", my404ERROR.length());
		rStr = resp.str();
		rStr+=my404ERROR;
	} else if(fdOut > 0){
		int realLen = (range == true) ? end - start + 1 : len;
		resp.header("Content-Length",UsefulFunctions::convertInt(realLen));
		resp.header("Last-Modified",mod);
		resp.header("Content-Type",type);
		rStr = resp.str();
	}
	if(CGIpath.length() > 0) {
		if(showDebug)
			cout << "CGI SCRIPT FOUND!!!!!!!!!" << endl;
		rStr = "HTTP/1.0 200 OK\r\nMIME-Version:1.0\r\n";

		// ********************************************

		cout << "LOOK AT LINES AFTER HERE!" << endl;
		std::vector<string> headerVector;
		GetHeaderLines(headerVector, req, true); // Read in the header lines, changing the format
		cout << "WAS THERE ANYTHING COOL??????" << endl;


		// ********************************************


	}

	if(isDir) {
		resp.header("Content-Length",dirs.length());
		rStr = resp.str();
		rStr += dirs;
	}
	if(showDebug)
		cout << "Sending: " << rStr << endl;

	ptr = rStr.c_str();
	int nwritten;
	int nleft = rStr.length();
	while (nleft) {
		if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
			if (errno == EINTR) {
				// the socket call was interrupted -- try again
				continue;
			} else {
				// an error occurred, so break out
				perror("send");
				return false;
			}
		} else if (nwritten == 0) {
			// the socket is closed
			return false;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}

	//Send file
	if(fdOut > 0){
		if(!head){
			if(range){
				off_t offset;
				offset = start;
				sendfile(client,fdOut,&offset,end-start+1);
			}else {
				sendfile(client,fdOut,NULL,len);
			}
		}
		close(fdOut);
	}
	return true;
}


