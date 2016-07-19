/*  
 *   *  hello-1.c - The simplest kernel module.
 *    */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/inet.h>

int init_module(void)
{
	int rc;
	struct socket *sock, *cli_socket;
	struct sockaddr_in address;

	rc = sock_create_kern(AF_INET, SOCK_STREAM, 0, &sock);
	if (rc != 0) {
		printk(KERN_ERR "Socket creation failed");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( (unsigned short)54668);

	rc = kernel_bind(sock, (struct sockaddr *)&address, sizeof(address)); 
	if (rc != 0) {
		printk(KERN_ERR "Socket Bind failed:%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "Socket bind success!!");

	rc = kernel_listen(sock, 5); 
	if (rc != 0) {
		printk(KERN_ERR "Failed to listen:%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "I am listening!!");

	rc = kernel_accept(sock, &cli_socket, 0);
	if (rc != 0) {
		printk(KERN_ERR "Kerne is not accptin :| :%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "Please go on mortal!!");

	/* 
 * 	 * A non 0 return means init_module failed; module can't be loaded. 
 * 	 	 */
	sock_release(sock);	
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}
