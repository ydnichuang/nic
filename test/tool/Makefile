CXX = g++
DESTDIR	=
SDKDIR	= /opt/yusur/hados
TARGET = doe_tool

# 源文件列表
SRC := $(wildcard *.c)

# 目标文件列表
OBJ := $(patsubst %.c, %.o,$(SRC))

# 编译目标文件
%.o: $(SRC_DIR)/%.c
	$(CXX) -c $< -o $@

# 链接生成可执行文件
$(TARGET): $(OBJ)
	$(CXX) $^ -L${SDKDIR}/lib -lhados_doe -g -o $@

clean:
	rm -f *.o doe_tool

install:
	mkdir -p ${DESTDIR}$(SDKDIR)/bin
	cp doe_tool *.sh ${DESTDIR}$(SDKDIR)/bin
