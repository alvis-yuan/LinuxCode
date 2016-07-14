#include <errno.h>  
#include <string.h>  
#include <fcntl.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <event.h>  
#include <evhttp.h>  
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
void httpserver_handler(struct evhttp_request *req, void *arg)  
{  
    int fd=open("./test.html",O_RDONLY);
    const char *cmdtype;   
    struct evbuffer *buf = evbuffer_new();  
    if (buf == NULL)   
    {  
        printf("evbuffer_new error !\n");  
        return;  
    }  
      
    switch (evhttp_request_get_command(req))  
    {    
        case EVHTTP_REQ_GET:    cmdtype = "GET";    break;    
        case EVHTTP_REQ_POST:   cmdtype = "POST";   break;    
        case EVHTTP_REQ_HEAD:   cmdtype = "HEAD";   break;    
        case EVHTTP_REQ_PUT:    cmdtype = "PUT";    break;    
        case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;    
        case EVHTTP_REQ_OPTIONS:cmdtype = "OPTIONS";break;    
        case EVHTTP_REQ_TRACE:  cmdtype = "TRACE";  break;    
        case EVHTTP_REQ_CONNECT:cmdtype = "CONNECT";break;    
        case EVHTTP_REQ_PATCH:  cmdtype = "PATCH";  break;    
        default: cmdtype = "unknown"; break;    
    }  
  
    printf("%s url=%s \n", cmdtype, evhttp_request_get_uri(req));  
  
    //evbuffer_add_printf(buf,"<html>\n"  
    //        "<head>\n"  
    //        "  <title>Libevnet Test</title>\n"            
    //        "</head>\n"  
    //        "<body>\n"  
    //        "  <h1>Hello world ,This is a Libenvet Test !</h1>\n"            
    //        "</body>\n"  
    //        "</html>\n");
    evbuffer_read(buf,fd,75);
    evhttp_send_reply(req, 200, "OK", buf);    
}   
   
int httpserver_bindsocket(int port, int backlog)  
{  
    int ret;  
    int fd;  
    fd = socket(AF_INET, SOCK_STREAM, 0);  
    if (fd < 0)   
    {  
        printf("socket error !\n");  
        return -1;  
    }  
  
    int opt = 1;  
    //ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int));  
  
    struct sockaddr_in addr;  
    memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    addr.sin_addr.s_addr =inet_addr("192.168.84.128");  
    addr.sin_port = htons(port);  
  
    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));  
    if (ret < 0)   
    {     
        printf("bind error !\n");  
        return -1;  
    }  
    ret = listen(fd, backlog);  
    if (ret < 0)   
    {  
        printf("listen error !\n");  
        return -1;  
    }  
  
    int flags;  
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0  
        || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)  
    {  
        printf("fcntl error !\n");  
        return -1;  
    }  
    return fd;  
}  
   
int httpserver_start(int port, int backlog)  
{  
    int ret;  
    int fd = httpserver_bindsocket(port, backlog);  
    if (fd < 0)   
    {     
        return -1;  
        printf("httpserver_bindsocket error !\n");  
    }  
    struct event_base *base = event_init();  
    if (base == NULL)   
    {  
        printf("event_init error !\n");  
        return -1;        
    }  
    struct evhttp *httpd = evhttp_new(base);  
    if (httpd == NULL)   
    {  
        printf("enhttp_new error !\n");  
        return -1;  
    }  
    ret = evhttp_accept_socket(httpd, fd);  
    if (ret != 0)   
    {  
        printf("evhttp_accept_socket error \n");  
        return -1;  
    }  
    evhttp_set_gencb(httpd, httpserver_handler, NULL);  
    event_base_dispatch(base);  
    return 0;  
}  
   
int main(void)  
{  
    httpserver_start(8000, 128);  
    return 0;  
}  
