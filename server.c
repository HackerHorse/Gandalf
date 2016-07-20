#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/inet.h>
#include<linux/kthread.h>
#include<linux/sched.h>

typedef struct kthread_sock_data {
	struct socket *sock;
	struct socket *cli_socket;
	struct task_struct *listener_thread;
} kthread_sock_data_t;

kthread_sock_data_t *thread_info;

static int accept_connection(void *connection_data)
{
	int rc;
	kthread_sock_data_t *connection_info = (kthread_sock_data_t*)connection_data;

	if (connection_info) {
		printk(KERN_ERR "Connection socket=%p", connection_info->sock);
	}
	rc = kernel_accept(connection_info->sock, &connection_info->cli_socket, 0);
	if (rc != 0) {
		printk(KERN_ERR "Kerne is not accptin :| :%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "Please go on mortal!!");
	return 0;
}

int init_module(void)
{
	int rc;

	struct sockaddr_in address;

	thread_info = kmalloc(sizeof(kthread_sock_data_t), GFP_KERNEL);
	if (!thread_info) {
		printk(KERN_ERR "Thread allocation failed:");
		return -1;
	}

	memset(thread_info, 0, sizeof(kthread_sock_data_t));

	rc = sock_create_kern(AF_INET, SOCK_STREAM, 0, &thread_info->sock);
	if (rc != 0) {
		printk(KERN_ERR "Socket creation failed");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( (unsigned short)54668);

	rc = kernel_bind(thread_info->sock, (struct sockaddr *)&address, sizeof(address)); 
	if (rc != 0) {
		printk(KERN_ERR "Socket Bind failed:%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "Socket bind success!!");

	rc = kernel_listen(thread_info->sock, 5); 
	if (rc != 0) {
		printk(KERN_ERR "Failed to listen:%d\n", rc);
		return -1;
	}
	printk(KERN_INFO "I am listening!!");

	thread_info->listener_thread = kthread_run(accept_connection, (void*)thread_info, "listener_thread");
	if (!thread_info->listener_thread) {
		printk(KERN_ERR "Failed to run listener thread");
		return -1;
	}
	
	return 0;
}

void cleanup_module(void)
{
	int rc;
	rc = kthread_stop(thread_info->listener_thread);
	if (rc != 0)
		printk(KERN_ERR "Some error in kernel accept connection.");

	sock_release(thread_info->sock);
	kfree(thread_info);	
}
