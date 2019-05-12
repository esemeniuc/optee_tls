//
// Created by eric on 05/05/19.
//

#ifndef OPTEE_EXAMPLE_HELLO_WORLD_PUBPRIV_H
#define OPTEE_EXAMPLE_HELLO_WORLD_PUBPRIV_H

char privkey[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                 "MIIEpAIBAAKCAQEAqpp2yp0KyupA6cFimf0/m+sL8bofP+SX9Q9xrQ3CABLboRyV\n"
                 "RUtI2wUGaBOFCNIDbvB/8YTZOLUwLWcKkPu0wn3v/U4NQvHDPWMtlP49YuvWIdxO\n"
                 "CLPvDDm55BXfiFMhjGQYXbjmkTsx9GAqYgljIEmma484tQw7xeU3xMMuwOhNA0Lp\n"
                 "YxM084uHXsfgvsl5c80CVEgVJ7biUkCkJwYk302NtUU59COV5dzPE8Ggdz3q31f4\n"
                 "iR/m81aAE/1//JS4BgD/MQdCj7b1KTsJcpbJuQrjvSLkhwXhYdrMSB4VOOyQnANc\n"
                 "wBO8FUJH85ynWNVu+hW2aEhrSqHkUOcrCkPFNQIDAQABAoIBAQCd6bgHN3Gj/Uz0\n"
                 "kfIow0c7lUdLCpKeW4NkbFU2b8OgudPdv01UBKl9QZt6fobncS2IZMioz+UZHm1n\n"
                 "uTcImFPqqxOXDcYsrwefLMwmW2aKx+0/DmKUn6inUk+EldKApyIohwYeymVxDK/i\n"
                 "mZjhgfrlCgcnDDzbKGWzecYzNM9z8o5+cK3CBW19fs4FEXsEEFJd8BEz3kYa1E3K\n"
                 "exm7WHlXKkfYDJJzrdCXET+30YSixVgTLAfgCpdp4ycAk4rJmUIEi6Lldj0HQd3t\n"
                 "kUHM0Tr0gc0xIgW/+LeErpkElFGuKGa50z4Hvb3DDeGfioicKczJ+VUeb93QjR4p\n"
                 "nk5Lah2JAoGBAN+2TmPvW9GsG6Uu3TUW+oQIrpRuCqCgZ2YrzMPmlLrNshN0Fc8b\n"
                 "zbz150SP+DZqzGI+yGhPC68t7YtxakQnLijMhcli/fgGfdD23G08CK9EVo3oMb5o\n"
                 "waQxDU8dQ7zhp9kxJUFQ81ojO+Nnq08+sD3NIP2rrFkL4thAfv1AZ523AoGBAMM5\n"
                 "5pGchQ+XKXo0TsVkQXtn2cWVwaVXkij1ztXRV/soUlBFieLHhXUepIMBxun+Fjcr\n"
                 "JgoaavOl7x4PjYW7M7eexhGZonqYwgLiuQY/RPycxn3PI3C890IOAvv319Qsc23m\n"
                 "+yRgZjgPzbSYleTSo1oyK+Yr3254MeGw7efpx3RzAoGAYoOpqaByUrbzxZ4Jk6N2\n"
                 "1kXseziqPWnlnryTA4G65ThHvwFTyUx5IUk4epzbtTr6nhj/r89/Wblj7hAXt8N5\n"
                 "NEY8dB6/sEVLioXrM79II8Z4Ry2FGlCPVXq0Xa6Jcdf9TkuyGD1JrNvvN4kji1YT\n"
                 "zmNSr+K1ltEBKDHeui9ucakCgYEAp4aSErXLgPtoPlOQnNZRUsJlH6ZIy8d/xuJY\n"
                 "ISdJZl7Nc9CYaCBqp3wqE7l8peLB0YmXVOgiiv4DWTpjaTShRppawVOjF6zqH2vK\n"
                 "9k9otZUax27urJmbW/WGHg4qCQoz60VgkXCfhtAY5zZgukD+qDGRdiyDyNofg9o9\n"
                 "7Nqxq30CgYBP248/8MQtRD4TAwi0y+WRXhbmBfFsyEzA/qsnhK0t2rxPCw/nMnbB\n"
                 "5rEQd+lQ2ZWe7RNOIMvNcnOQOWWS3331w9UuCB5NP7ZS8OF4sqFC0wySjAIRG9GR\n"
                 "uwNHKE5jfNrl+wx/Xy3hqA8flE1BHZDC09HI+C1o1p7/hpB/HiJQBg==\n"
                 "-----END RSA PRIVATE KEY-----";

char pubkey[] = "-----BEGIN RSA PUBLIC KEY-----\n"
                "MIIBCgKCAQEAqpp2yp0KyupA6cFimf0/m+sL8bofP+SX9Q9xrQ3CABLboRyVRUtI\n"
                "2wUGaBOFCNIDbvB/8YTZOLUwLWcKkPu0wn3v/U4NQvHDPWMtlP49YuvWIdxOCLPv\n"
                "DDm55BXfiFMhjGQYXbjmkTsx9GAqYgljIEmma484tQw7xeU3xMMuwOhNA0LpYxM0\n"
                "84uHXsfgvsl5c80CVEgVJ7biUkCkJwYk302NtUU59COV5dzPE8Ggdz3q31f4iR/m\n"
                "81aAE/1//JS4BgD/MQdCj7b1KTsJcpbJuQrjvSLkhwXhYdrMSB4VOOyQnANcwBO8\n"
                "FUJH85ynWNVu+hW2aEhrSqHkUOcrCkPFNQIDAQAB\n"
                "-----END RSA PUBLIC KEY-----";

//escape newline since json doesnt support mulitline
char pubkey_network[] = "-----BEGIN RSA PUBLIC KEY-----\\n"
                "MIIBCgKCAQEAqpp2yp0KyupA6cFimf0/m+sL8bofP+SX9Q9xrQ3CABLboRyVRUtI\\n"
                "2wUGaBOFCNIDbvB/8YTZOLUwLWcKkPu0wn3v/U4NQvHDPWMtlP49YuvWIdxOCLPv\\n"
                "DDm55BXfiFMhjGQYXbjmkTsx9GAqYgljIEmma484tQw7xeU3xMMuwOhNA0LpYxM0\\n"
                "84uHXsfgvsl5c80CVEgVJ7biUkCkJwYk302NtUU59COV5dzPE8Ggdz3q31f4iR/m\\n"
                "81aAE/1//JS4BgD/MQdCj7b1KTsJcpbJuQrjvSLkhwXhYdrMSB4VOOyQnANcwBO8\\n"
                "FUJH85ynWNVu+hW2aEhrSqHkUOcrCkPFNQIDAQAB\\n"
                "-----END RSA PUBLIC KEY-----";
#endif //OPTEE_EXAMPLE_HELLO_WORLD_PUBPRIV_H
