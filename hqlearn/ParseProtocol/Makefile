# 编译器设置
CC = g++
CFLAGS = -Wall -Wextra -std=c++98 -O2  # 遵循C++98标准，兼容C99
LDFLAGS = 

#头文件目录
INC			+=  -I./common \
				-I./hhbus_protocol

# 文件列表
CPPSRCS = $(wildcard ./*.cpp)
CPPSRCS += $(wildcard ./common/*.cpp)
CPPSRCS += $(wildcard ./hhbus_protocol/*.cpp)
OBJS = $(CPPSRCS:.cpp=.o)

HDRS = hhbus_protocol.h

# 目标程序
TARGET = hhbus_demo

# 默认目标
all: $(TARGET)

print:
	@echo "test:"
	@echo	"$(OBJS)"
# 链接
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(INC) $(OBJS) $(LDFLAGS)
	rm -rf $(OBJS)

# 编译
%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标
.PHONY: all clean