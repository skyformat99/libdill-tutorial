/* Little boring program to learn how libdill supports concurrency.
   This one uses the generator "pattern" */

#include <libdill.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

coroutine void boring(const char* msg, int ch)
{
    int i = 0;
    while(1) {
        char *buf = malloc(128);
        assert(buf);
        int n = snprintf(buf, 128, "%s %d", msg, i);
        assert(n >= 0);
        buf[n] = 0;
        int rc = chsend(ch, &buf, sizeof(&buf), -1);
        assert(rc == 0);
        msleep(now() + (rand() % 1000));
        ++i;
    }
}

int boring_gen(const char* msg)
{
    int ch = channel(sizeof(char *), 0);
    assert(ch >= 0);
    int cr = go(boring(msg, ch));
    assert(cr >= 0);
    return ch;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    printf("I'm listening.\n");
    int joe = boring_gen("Joe");
    int ann = boring_gen("Ann");
    for(int i = 0; i < 5; ++i) {
        char *msg = NULL;
        int rc = chrecv(joe, &msg, sizeof(msg), -1);
        assert(rc == 0);
        assert(msg);
        printf("%s\n", msg);
        free(msg);

        rc = chrecv(ann, &msg, sizeof(msg), -1);
        assert(rc == 0);
        assert(msg);
        printf("%s\n", msg);
        free(msg);
    }
    printf("You're boring, I'm leaving.\n");
    return 0;
}
