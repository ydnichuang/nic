#include <argp.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>

#include "../../modules/include/uapi_yusdoe.h"
#include "../../lib/include/hados_doe.h"

#include "tool.h"

#define TABLE_ID_INVALID	0xff
#define INDEX_INVALID		0xffffffff
int in_file;
void *in_buffer;

struct arguments {
	uint8_t tbl_id;
	enum yusur_doe_cmd_opcode opc;
	uint32_t index;
	char key[128];
	char dov[280];
	uint8_t input_key_len;
	uint16_t input_dov_len;
	char *in_file;
	char *out_file;
	uint32_t count_batch;
	uint8_t elem_size;

	/* param for create table */
	uint16_t key_len;
	uint16_t dov_len;	/* also can specify output len for read */
	uint16_t index_sram_size;
	uint32_t depth;

	/* param for cache */
	bool use_cache;
	bool shared_tbl;
	uint8_t cache_ways_num[17];
};

/* Program documentation. */
static char doc[] = "DOE test tool, Operation Code:\n"
			"\t0x01 RAW command\n"
			"\t0x10 Create array\n"
			"\t0x11 Delete array\n"
			"\t0x12 Create hash\n"
			"\t0x13 Delete hash\n"
			"\t0x20 Array load\n"
			"\t0x21 Array store\n"
			"\t0x24 Array load batch\n"
			"\t0x25 Array store batch\n"
			"\t0x26 Array read-clear\n"
			"\t0x2a Array read-clear batch\n"
			"\t0x30 Hash insert\n"
			"\t0x31 Hash delete\n"
			"\t0x32 Hash query\n"
			"\t0x34 Hash insert batch\n"
			"\t0x35 Hash delete batch\n"
			"\t0x36 Hash query batch\n";

static struct argp_option options[] = {
	{"opcode",	'o',	"Hex",		0, "DOE operation code"},
	{"tableid",	't',	"Dec",		0, "Target table ID"},
	{"index",	'i',	"Dec",		0, "Index of Array table"},
	{"key",		'k',	"Hex array",	0, "Key of Hash table"},
	{"dov",		'v',	"Hex array",	0, "Data/Value of Array/Hash table"},
	{"key_len",	'K',	"Size",		0, "Key of Hash table"},
	{"dov_len",	'V',	"Size",		0, "Data/Value of Array/Hash table"},
	{"depth",	'd',	"Size",		0, "Depth of Array/Hash table"},
	{"in_batch",	'I',	"File name",	0, "Input batch file"},
	{"out_batch",	'O',	"File name",	0, "Ouput batch file(read result)"},
	{"count_batch",	'c',	"Count",	0, "Count of sub cmd for batch"},
	{"elem_size",	'e',	"Size",		0, "Ouput element size for batch"},
	{"shared_table",'s', 	NULL, 		0, "cache type config"},
	{"cache", 	'a', 	"Hex array", 	0, "data of cache way config"},
	{ 0 },
};

static int str_to_hex_array(char *str, char *data, uint32_t size)
{
	char *ptr, *next;
	int i = 0;

	ptr = strtok_r(str, " ", &next);
	while (ptr != NULL && i < size) {
		data[i] = strtol(ptr, NULL, 16);
		i++;
		ptr = strtok_r(NULL, " ", &next);
	}

	return i;
}

static int str_to_dec_array(char *str, char *data, uint32_t size)
{
	char *ptr, *next;
	int i = 0;

	ptr = strtok_r(str, " ", &next);
	while (ptr != NULL && i < size) {
		data[i] = strtol(ptr, NULL, 10);
		i++;
		ptr = strtok_r(NULL, " ", &next);
	}

	return i;
}

static int read_infile(char *file_name, void **buffer)
{
	int fd;
	uint32_t size;

	fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "%s is not exist!\n", file_name);
		return -EBADF;
	}

	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, 0);

	*buffer = malloc(size);
	if (!*buffer) {
		close(fd);
		return -ENOMEM;
	}

	if (read(fd, *buffer, size) != size) {
		free(*buffer);
		close(fd);
		return -EBADF;
	}

	return fd;
}

static void *release_infile(int fd, void *buffer)
{
	close(fd);
	free(buffer);
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *argu = (struct arguments *)(state->input);

	switch (key) {
	case 'o':
		argu->opc = strtol(arg, NULL, 16);
		fprintf(stdout, "opcode: 0x%x\n", argu->opc);
		break;
	case 't':
		argu->tbl_id = atoi(arg);
		fprintf(stdout, "table_id: %d\n", argu->tbl_id);
		break;
	case 'i':
		argu->index = atoi(arg);
		fprintf(stdout, "index: %d\n", argu->index);
		break;
	case 'k':
		argu->input_key_len = str_to_hex_array(arg, argu->key, sizeof(argu->key));

		fprintf(stdout, "Key: ");
		for (int i = 0; i < argu->input_key_len; i++)
			fprintf(stdout, "%02x ", (uint8_t)argu->key[i]);
		fprintf(stdout, "\n");
		break;
	case 'v':
		argu->input_dov_len = str_to_hex_array(arg, argu->dov, sizeof(argu->dov));

		fprintf(stdout, "DoV: ");
		for (int i = 0; i < argu->input_dov_len; i++)
			fprintf(stdout, "%02x ", (uint8_t)argu->dov[i]);
		fprintf(stdout, "\n");
		break;
	case 'K':
		argu->key_len = atoi(arg);
		fprintf(stdout, "key_len: %d\n", argu->key_len);
		break;
	case 'V':
		argu->dov_len = atoi(arg);
		fprintf(stdout, "dov_len: %d\n", argu->dov_len);
		break;
	case 'd':
		argu->depth = atoi(arg);
		fprintf(stdout, "depth: %d\n", argu->depth);
		break;
	case 'I':
		argu->in_file = arg;
		fprintf(stdout, "input file: %s\n", argu->in_file);

		in_file = read_infile(argu->in_file, &in_buffer);
		if (in_file < 0) {
			fprintf(stdout, "Failed to open input file!\n");
			return in_file;
		}

		break;
	case 'O':
		argu->out_file = arg;
		fprintf(stdout, "output file: %s\n", argu->out_file);
		break;
	case 'c':
		argu->count_batch = atoi(arg);
		fprintf(stdout, "count of batch: %d\n", argu->count_batch);
		break;
	case 'e':
		argu->elem_size = atoi(arg);
		fprintf(stdout, "output element size: %d\n", argu->elem_size);
		break;
	case 's':
		argu->shared_tbl = true;
		break;
	case 'a':
		argu->use_cache = true;
		str_to_dec_array(arg, argu->cache_ways_num, sizeof(argu->cache_ways_num));
		fprintf(stdout, "l1 cache ways: ");
		for (int i = 0; i < 16; i++)
			fprintf(stdout, "%02d ", (uint8_t)argu->cache_ways_num[i]);

		fprintf(stdout, "\n");
		fprintf(stdout, "l2 cache ways: %d", argu->cache_ways_num[16]);

		fprintf(stdout, "\n");
		break;
	default:
		break;
	}

	return 0;
}

/* argp parser. */
static struct argp argp = {
	.options = options,
	.parser = parse_opt,
	.args_doc = NULL,
	.doc = doc,
	.children = NULL,
	.help_filter = NULL,
	.argp_domain = NULL,
};

static int init_outbuffer(void **buffer, uint32_t size)
{
	*buffer = malloc(size);
	if (!*buffer)
		return -ENOMEM;

	memset(*buffer, 0, size);

	return 0;
}

static int write_release_outbuffer(char *file_name, void *buffer, uint32_t size)
{
	int fd, ret;

	fd = open(file_name, O_RDWR | O_CREAT, 0666);
	if (fd == -1) {
		fprintf(stderr, "%s open failed!\n", file_name);
		return -EBADF;
	}

	ret = write(fd, buffer, size);
	if (ret != size) {
		fprintf(stderr, "%s write %d failed!\n", file_name, size);
		close(fd);
		return -EIO;
	}

	free(buffer);
	close(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	struct arguments argu = {0};
	struct yusdoe_sw_cmd cmd;
	struct hw_advance_cfg cache_cfg;
	struct hw_advance_cfg *tmp_cache = NULL;
	void *out_buffer;
	int fd;
	int is_read;
	int cnt, ret, i;

	if (memcmp(argv[1], "case1", strlen("case1")) == 0) {
		return func_case1_main(argc, argv);
	} else if (memcmp(argv[0], "case2", strlen("case2")) == 0) {
		return func_case2_main(argc, argv);	
	}

	argu.tbl_id = TABLE_ID_INVALID;
	argu.index = INDEX_INVALID;
	argp_parse(&argp, argc, argv, 0, 0, &argu);

	/* opcode and tbl_id is necessary */
	if (!argu.opc || (argu.opc != YUSDOE_SW_RAW_CMD &&
			  argu.tbl_id == TABLE_ID_INVALID)) {
		fprintf(stderr, "Please Input opcode && tbl_id\n\n");
		return -EINVAL;
	}

	fd = hados_doe_request_fd();
	if (fd == -1) {
		fprintf(stderr, "Open DOE failed!\n\n");
		return -EBADF;
	}

	switch (argu.opc) {
	case YUSDOE_SW_RAW_CMD:
		if (!argu.input_dov_len) {
			fprintf(stderr, "Please input RAW cmd!\n\n");
			return -EINVAL;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.opcode = argu.opc;
		cmd.tbl_id = (uint8_t)argu.tbl_id;
		/* Check if there need read value return */
		if (argu.dov[0] == YUSDOE_SW_ARRAY_LOAD ||
		    argu.dov[0] == YUSDOE_SW_ARRAY_READ_CLEAR ||
		    argu.dov[0] == YUSDOE_SW_ARRAY_READ ||
		    argu.dov[0] == YUSDOE_SW_HASH_QUERY) {
			cmd.is_read = 1;
			is_read = 1;
		}

		cmd.cmd = argu.dov;
		cmd.cmd_size = argu.input_dov_len;

		ret = ioctl(fd, YUSDOE_SEND_CMD, &cmd);
		if (ret) {
			fprintf(stderr, "command exe error! %d\n", errno);
			goto out;
		}

		if (!cmd.succeed) {
			fprintf(stderr, "command fail!\n");
			ret = -EIO;
			goto out;
		}

		if (argu.dov[0] == YUSDOE_SW_HASH_QUERY)
			memcpy(argu.dov, cmd.value, 128);
		else
			memcpy(argu.dov, cmd.data, 256);

		break;

	case YUSDOE_SW_CREATE_HASH:
		if (!argu.depth || !argu.key_len || !argu.dov_len) {
			fprintf(stderr, "Create Hash table need param -d, -K and -V\n");
			ret = -EINVAL;
			goto out;
		}

		if (argu.use_cache) {
			cache_cfg.shared_tbl = argu.shared_tbl;
			for (i = 0; i < 16; ++i)
				cache_cfg.l1_cache_ways[i] = argu.cache_ways_num[i];
			cache_cfg.l2_cache_ways = argu.cache_ways_num[16];

			tmp_cache = &cache_cfg;
		}

		ret = hados_doe_create_hashtbl(fd, argu.tbl_id, argu.depth,
					    argu.key_len, argu.dov_len, tmp_cache);
		break;

	case YUSDOE_SW_CREATE_ARRAY:
		if (!argu.depth || !argu.dov_len) {
			fprintf(stderr, "Create array table need param -d and -V\n");
			ret = -EINVAL;
			goto out;
		}

		if (argu.use_cache) {
			cache_cfg.shared_tbl = argu.shared_tbl;
			for (i = 0; i < 16; ++i)
				cache_cfg.l1_cache_ways[i] = argu.cache_ways_num[i];
			cache_cfg.l2_cache_ways = argu.cache_ways_num[16];

			tmp_cache = &cache_cfg;
		}

		ret = hados_doe_create_arraytbl(fd, argu.tbl_id, argu.depth,
					     argu.dov_len, tmp_cache);
		break;

	case YUSDOE_SW_DELETE_HASH:
		ret = hados_doe_delete_hashtbl(fd, argu.tbl_id);
		break;

	case YUSDOE_SW_DELETE_ARRAY:
		ret = hados_doe_delete_arraytbl(fd, argu.tbl_id);
		break;

	case YUSDOE_SW_ARRAY_LOAD:
		if (argu.index == INDEX_INVALID) {
			fprintf(stderr, "Array load need param -i\n");
			ret = -EINVAL;
			goto out;
		}

		if (!argu.dov_len)
			fprintf(stdout, "Warning: No data Len specified!\n");

		is_read = 1;
		ret = hados_doe_array_load(fd, argu.tbl_id, argu.index, argu.dov, 128);
		break;

	case YUSDOE_SW_ARRAY_READ_CLEAR:
		if (argu.index == INDEX_INVALID) {
			fprintf(stderr, "Array read clear need param -i\n");
			ret = -EINVAL;
			goto out;
		}

		if (!argu.dov_len)
			fprintf(stdout, "Warning: No data Len specified!\n");

		is_read = 1;
		ret = hados_doe_array_read_clear(fd, argu.tbl_id, argu.index, argu.dov, 128);
		break;

	case YUSDOE_SW_ARRAY_STORE:
		if (argu.index == INDEX_INVALID || !argu.input_dov_len) {
			fprintf(stderr, "Array store need param -i and -v\n");
			ret = -EINVAL;
			goto out;
		}

		ret = hados_doe_array_store(fd, argu.tbl_id, argu.index, argu.dov, 128);
		break;

	case YUSDOE_SW_HASH_QUERY:
		if (!argu.input_key_len) {
			fprintf(stderr, "Hash query need param -k\n");
			ret = -EINVAL;
			goto out;
		}

		if (!argu.dov_len)
			fprintf(stdout, "Warning: No data Len specified!\n");

		is_read = 1;
		ret = hados_doe_hash_query(fd, argu.tbl_id, argu.key, 128, argu.dov, 128);
		break;

	case YUSDOE_SW_HASH_DELETE:
		if (!argu.input_key_len) {
			fprintf(stderr, "Hash delete need param -k\n");
			ret = -EINVAL;
			goto out;
		}

		ret = hados_doe_hash_delete(fd, argu.tbl_id, argu.key, 128);
		break;

	case YUSDOE_SW_HASH_INSERT:
		if (!argu.input_key_len || !argu.input_dov_len) {
			fprintf(stderr, "Hash insert need param -k and -v\n");
			ret = -EINVAL;
			goto out;
		}

		ret = hados_doe_hash_insert(fd, argu.tbl_id, argu.key, 128, argu.dov, 128);
		break;

	case YUSDOE_SW_ARRAY_STORE_BATCH:
		if (!argu.in_file || !argu.count_batch) {
			fprintf(stderr, "Array store batch need param -I and -c\n");
			ret = -EINVAL;
			goto out;
		}

		cnt = hados_doe_array_store_batch(fd, argu.tbl_id, argu.count_batch, in_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	case YUSDOE_SW_ARRAY_LOAD_BATCH:
		if (!argu.in_file || !argu.count_batch ||
		    !argu.out_file || !argu.elem_size) {
			fprintf(stderr, "Array load batch need param -I, -O, -c and -e\n");
			ret = -EINVAL;
			goto out;
		}

		ret = init_outbuffer(&out_buffer,
				     argu.elem_size * argu.count_batch);
		if (ret)
			goto out;

		cnt = hados_doe_array_load_batch(fd, argu.tbl_id, argu.count_batch,
					      in_buffer, out_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	case YUSDOE_SW_ARRAY_READ_CLEAR_BATCH:
		if (!argu.in_file || !argu.count_batch ||
		    !argu.out_file || !argu.elem_size) {
			fprintf(stderr, "Array read clear batch need param -I, -O, -c and -e\n");
			ret = -EINVAL;
			goto out;
		}

		ret = init_outbuffer(&out_buffer,
				     argu.elem_size * argu.count_batch);
		if (ret)
			goto out;

		cnt = hados_doe_array_readclear_batch(fd, argu.tbl_id,
						    argu.count_batch,
						    in_buffer, out_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	case YUSDOE_SW_HASH_INSERT_BATCH:
		if (!argu.in_file || !argu.count_batch) {
			fprintf(stderr, "Hash insert batch need param -I and -c\n");
			ret = -EINVAL;
			goto out;
		}

		cnt = hados_doe_hash_insert_batch(fd, argu.tbl_id, argu.count_batch, in_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	case YUSDOE_SW_HASH_DELETE_BATCH:
		if (!argu.in_file || !argu.count_batch) {
			fprintf(stderr, "Hash delete batch need param -I and -c\n");
			ret = -EINVAL;
			goto out;
		}

		cnt = hados_doe_hash_delete_batch(fd, argu.tbl_id, argu.count_batch, in_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	case YUSDOE_SW_HASH_QUERY_BATCH:
		if (!argu.in_file || !argu.count_batch ||
		    !argu.out_file || !argu.elem_size) {
			fprintf(stderr, "Hash query batch need param -I, -O, -c and -e\n");
			ret = -EINVAL;
			goto out;
		}

		ret = init_outbuffer(&out_buffer,
				     argu.elem_size * argu.count_batch);
		if (ret)
			goto out;

		cnt = hados_doe_hash_query_batch(fd, argu.tbl_id, argu.count_batch,
					      in_buffer, out_buffer);
		if (cnt != argu.count_batch) {
			fprintf(stderr, "Batch failed with %d/%d\n",
					argu.count_batch - cnt, argu.count_batch);
			ret = -EIO;
		}
		break;

	default:
		fprintf(stderr, "Please Input CORRECT OPCODE\n");
		return -EINVAL;
	}

	if (ret) {
		fprintf(stderr, "command exe error %d!\n", ret);
	} else if (argu.out_file) {
		/* for (int j = 0; j < argu.count_batch; j++) { */
		/* 	fprintf(stdout, "Item %04x: ", j); */
		/* 	for (int i = 0; i < argu.elem_size; i++) */
		/* 		fprintf(stdout, "%02x ", *(uint8_t *)(out_buffer + */
		/* 					j * argu.elem_size + i)); */
		/* 	fprintf(stdout, "\n"); */
		/* } */
		write_release_outbuffer(argu.out_file, out_buffer,
				argu.elem_size * argu.count_batch);
	} else if (is_read) {
		fprintf(stdout, "Succeed with output:\n");
		for (int i = 0; i < (argu.dov_len ? : 256); i++)
			fprintf(stdout, "%02x ", *(uint8_t *)(argu.dov + i));
		fprintf(stdout, "\n");
	} else {
		fprintf(stdout, "Succeed!\n");
	}

out:
	if (argu.in_file)
		release_infile(in_file, in_buffer);
	hados_doe_release_fd(fd);
	fprintf(stdout, "\n");

	return ret;
}
