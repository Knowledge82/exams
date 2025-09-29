#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

static node	*parse_atomic(char **s);
static node	*parse_multiply(char **s);
static node	*parse_add(char **s);
node	*parse_expr(char *s);

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");
}

int accept(char **s, char c)
{
    if (**s && **s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

//... MY FUNCS:

static node	*parse_atomic(char **s)// parse num, ( )
{
	if (isdigit((unsigned char)**s))
	{
		node n =
		{
			.type = VAL,
			.val = **s - '0',
			.l = NULL,
			.r = NULL
		};
		(*s)++;
		return new_node(n);
	}
	if (accept(s, '('))
	{
		node *res = parse_add(s);
		if (!res)
			return (NULL);
		if (!expect(s, ')'))
		{
			destroy_tree(res);
			return (NULL);
		}
		else
			return (res);
	}
	else
	{
		unexpected(**s);
		return (NULL);
	}
}

static node	*parse_multiply(char **s)
{
	node *left = parse_atomic(s);
	if (!left)
		return (NULL);
	while (accept(s, '*'))
	{
		node *right = parse_atomic(s);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		node n =
		{
			.type = MULTI,
			.l = left,
			.r = right
		};
		left = new_node(n);
		if (!left)
		{
			destroy_tree(right);
			return (NULL);
		}
	}
	return (left);
}

static node	*parse_add(char **s)
{
	node *left = parse_multiply(s);
	if (!left)
		return (NULL);
	while (accept(s, '+'))
	{
		node *right = parse_multiply(s);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		node n =
		{
			.type = ADD,
			.l = left,
			.r = right
		};
		left = new_node(n);
		if (!left)
		{
			destroy_tree(right);
			return (NULL);
		}
	}
	return (left);

}

node    *parse_expr(char *s)
{
    //...
	char	*ptr = s;
	node	*ret = parse_add(&ptr);

	if (!ret)
		return (NULL);

    if (*ptr) 
    {
        destroy_tree(ret);
		unexpected(*ptr);
        return (NULL);
    }
    return (ret);
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
}
