case 0x00:
    break;
case 0x01:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_ORA, 0);
    break;
case 0x02:
    break;
case 0x03:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0x04:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x05:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_ORA, 0);
    break;
case 0x06:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x07:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x08:
    break;
case 0x09:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_ORA, 0);
    break;
case 0x0a:
    break;
case 0x0b:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x0c:
    break;
case 0x0d:
    X(SYMOP_OP_ORA, 0);
    break;
case 0x0e:
    break;
case 0x0f:
    break;
case 0x10:
    break;
case 0x11:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_ORA, 0);
    break;
case 0x12:
    break;
case 0x13:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0x14:
    break;
case 0x15:
    X(SYMOP_OP_ORA, 0);
    break;
case 0x16:
    break;
case 0x17:
    break;
case 0x18:
    break;
case 0x19:
    X(SYMOP_OP_ORA, 0);
    break;
case 0x1a:
    break;
case 0x1b:
    break;
case 0x1c:
    break;
case 0x1d:
    X(SYMOP_OP_ORA, 0);
    break;
case 0x1e:
    break;
case 0x1f:
    break;
case 0x20:
    break;
case 0x21:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_AND, 0);
    break;
case 0x22:
    break;
case 0x23:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0x24:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x25:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_AND, 0);
    break;
case 0x26:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x27:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x28:
    break;
case 0x29:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_AND, 0);
    break;
case 0x2a:
    break;
case 0x2b:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x2c:
    break;
case 0x2d:
    X(SYMOP_OP_AND, 0);
    break;
case 0x2e:
    break;
case 0x2f:
    break;
case 0x30:
    break;
case 0x31:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_AND, 0);
    break;
case 0x32:
    break;
case 0x33:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0x34:
    break;
case 0x35:
    X(SYMOP_OP_AND, 0);
    break;
case 0x36:
    break;
case 0x37:
    break;
case 0x38:
    break;
case 0x39:
    X(SYMOP_OP_AND, 0);
    break;
case 0x3a:
    break;
case 0x3b:
    break;
case 0x3c:
    break;
case 0x3d:
    X(SYMOP_OP_AND, 0);
    break;
case 0x3e:
    break;
case 0x3f:
    break;
case 0x40:
    break;
case 0x41:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_EOR, 0);
    break;
case 0x42:
    break;
case 0x43:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0x44:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x45:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_EOR, 0);
    break;
case 0x46:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x47:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x48:
    break;
case 0x49:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_EOR, 0);
    break;
case 0x4a:
    break;
case 0x4b:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x4c:
    break;
case 0x4d:
    X(SYMOP_OP_EOR, 0);
    break;
case 0x4e:
    break;
case 0x4f:
    break;
case 0x50:
    break;
case 0x51:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_EOR, 0);
    break;
case 0x52:
    break;
case 0x53:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0x54:
    break;
case 0x55:
    X(SYMOP_OP_EOR, 0);
    break;
case 0x56:
    break;
case 0x57:
    break;
case 0x58:
    break;
case 0x59:
    X(SYMOP_OP_EOR, 0);
    break;
case 0x5a:
    break;
case 0x5b:
    break;
case 0x5c:
    break;
case 0x5d:
    X(SYMOP_OP_EOR, 0);
    break;
case 0x5e:
    break;
case 0x5f:
    break;
case 0x60:
    break;
case 0x61:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_ADC, 0);
    break;
case 0x62:
    break;
case 0x63:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0x64:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x65:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_ADC, 0);
    break;
case 0x66:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x67:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x68:
    break;
case 0x69:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_ADC, 0);
    break;
case 0x6a:
    break;
case 0x6b:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x6c:
    break;
case 0x6d:
    X(SYMOP_OP_ADC, 0);
    break;
case 0x6e:
    break;
case 0x6f:
    break;
case 0x70:
    break;
case 0x71:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_ADC, 0);
    break;
case 0x72:
    break;
case 0x73:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0x74:
    break;
case 0x75:
    X(SYMOP_OP_ADC, 0);
    break;
case 0x76:
    break;
case 0x77:
    break;
case 0x78:
    break;
case 0x79:
    X(SYMOP_OP_ADC, 0);
    break;
case 0x7a:
    break;
case 0x7b:
    break;
case 0x7c:
    break;
case 0x7d:
    X(SYMOP_OP_ADC, 0);
    break;
case 0x7e:
    break;
case 0x7f:
    break;
case 0x80:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x81:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_STA, 0);
    break;
case 0x82:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x83:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0x84:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x85:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_STA, 0);
    break;
case 0x86:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x87:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0x88:
    break;
case 0x89:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_STA, 0);
    break;
case 0x8a:
    break;
case 0x8b:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0x8c:
    break;
case 0x8d:
    X(SYMOP_OP_STA, 0);
    break;
case 0x8e:
    break;
case 0x8f:
    break;
case 0x90:
    break;
case 0x91:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_STA, 0);
    break;
case 0x92:
    break;
case 0x93:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0x94:
    break;
case 0x95:
    X(SYMOP_OP_STA, 0);
    break;
case 0x96:
    break;
case 0x97:
    break;
case 0x98:
    break;
case 0x99:
    X(SYMOP_OP_STA, 0);
    break;
case 0x9a:
    break;
case 0x9b:
    break;
case 0x9c:
    break;
case 0x9d:
    X(SYMOP_OP_STA, 0);
    break;
case 0x9e:
    break;
case 0x9f:
    break;
case 0xa0:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xa1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_LDA, 0);
    break;
case 0xa2:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xa3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0xa4:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xa5:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_LDA, 0);
    break;
case 0xa6:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xa7:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xa8:
    break;
case 0xa9:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_LDA, 0);
    break;
case 0xaa:
    break;
case 0xab:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xac:
    break;
case 0xad:
    X(SYMOP_OP_LDA, 0);
    break;
case 0xae:
    break;
case 0xaf:
    break;
case 0xb0:
    break;
case 0xb1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_LDA, 0);
    break;
case 0xb2:
    break;
case 0xb3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0xb4:
    break;
case 0xb5:
    X(SYMOP_OP_LDA, 0);
    break;
case 0xb6:
    break;
case 0xb7:
    break;
case 0xb8:
    break;
case 0xb9:
    X(SYMOP_OP_LDA, 0);
    break;
case 0xba:
    break;
case 0xbb:
    break;
case 0xbc:
    break;
case 0xbd:
    X(SYMOP_OP_LDA, 0);
    break;
case 0xbe:
    break;
case 0xbf:
    break;
case 0xc0:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xc1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_CMP, 0);
    break;
case 0xc2:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xc3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0xc4:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xc5:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_CMP, 0);
    break;
case 0xc6:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xc7:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xc8:
    break;
case 0xc9:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_CMP, 0);
    break;
case 0xca:
    break;
case 0xcb:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xcc:
    break;
case 0xcd:
    X(SYMOP_OP_CMP, 0);
    break;
case 0xce:
    break;
case 0xcf:
    break;
case 0xd0:
    break;
case 0xd1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_CMP, 0);
    break;
case 0xd2:
    break;
case 0xd3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0xd4:
    break;
case 0xd5:
    X(SYMOP_OP_CMP, 0);
    break;
case 0xd6:
    break;
case 0xd7:
    break;
case 0xd8:
    break;
case 0xd9:
    X(SYMOP_OP_CMP, 0);
    break;
case 0xda:
    break;
case 0xdb:
    break;
case 0xdc:
    break;
case 0xdd:
    X(SYMOP_OP_CMP, 0);
    break;
case 0xde:
    break;
case 0xdf:
    break;
case 0xe0:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xe1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    X(SYMOP_OP_SBC, 0);
    break;
case 0xe2:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xe3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_X, 1);
    break;
case 0xe4:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xe5:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    X(SYMOP_OP_SBC, 0);
    break;
case 0xe6:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xe7:
    X(SYMOP_ADDR_ZEROPAGE, 1);
    break;
case 0xe8:
    break;
case 0xe9:
    X(SYMOP_ADDR_IMM, 1);
    X(SYMOP_OP_SBC, 0);
    break;
case 0xea:
    break;
case 0xeb:
    X(SYMOP_ADDR_IMM, 1);
    break;
case 0xec:
    break;
case 0xed:
    X(SYMOP_OP_SBC, 0);
    break;
case 0xee:
    break;
case 0xef:
    break;
case 0xf0:
    break;
case 0xf1:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    X(SYMOP_OP_SBC, 0);
    break;
case 0xf2:
    break;
case 0xf3:
    X(SYMOP_ADDR_ZEROPAGE_INDIRECT_Y, 1);
    break;
case 0xf4:
    break;
case 0xf5:
    X(SYMOP_OP_SBC, 0);
    break;
case 0xf6:
    break;
case 0xf7:
    break;
case 0xf8:
    break;
case 0xf9:
    X(SYMOP_OP_SBC, 0);
    break;
case 0xfa:
    break;
case 0xfb:
    break;
case 0xfc:
    break;
case 0xfd:
    X(SYMOP_OP_SBC, 0);
    break;
case 0xfe:
    break;
case 0xff:
    break;
