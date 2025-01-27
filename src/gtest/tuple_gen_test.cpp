/*
 Wenxian Li 
 
 Cisco Systems, Inc.
*/

/*
Copyright (c) 2015-2015 Cisco Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "../bp_sim.h"
#include <common/gtest.h>
#include <common/basic_utils.h>

/* TEST case for CClientInfo*/

class CClientInfoUT {
public:
    CClientInfoUT(CClientInfo *a) {
        obj = a;
    }

    uint16_t m_head_port_get() {
        return obj->m_head_port;
    }

    void m_head_port_set(uint16_t head) {
        obj->m_head_port = head;
    }

    bool is_port_legal(uint16_t port) {
        return obj->is_port_legal(port);
    }
    bool is_port_free(uint16_t port) {
        return obj->is_port_available(port);
    }
    void m_bitmap_port_set_bit(uint16_t port) {
        obj->m_bitmap_port[port] = 1;
    }
    void m_bitmap_port_reset_bit(uint16_t port) {
        obj->m_bitmap_port[port] = 0;
    }
    uint8_t m_bitmap_port_get_bit(uint16_t port) {
        return obj->m_bitmap_port[port];
    }
    void get_next_free_port_by_bit() {
        obj->get_next_free_port_by_bit();
    }

    CClientInfo *obj;
};

TEST(CClientInfoTest, Constructors) {
    CClientInfo c;
    CClientInfoUT client(&c);
    EXPECT_EQ(MIN_PORT, client.m_head_port_get());

    CClientInfo c2;
    CClientInfoUT client2(&c2);
    EXPECT_EQ(MIN_PORT, client2.m_head_port_get());
}

TEST(CClientInfoTest, is_port_legal) {
    CClientInfo c;
    CClientInfoUT client(&c);
    EXPECT_TRUE(client.is_port_legal(MIN_PORT));
    EXPECT_FALSE(client.is_port_legal(MIN_PORT-1));
    EXPECT_TRUE(client.is_port_legal(MAX_PORT-1));
    EXPECT_FALSE(client.is_port_legal(MAX_PORT));
    EXPECT_FALSE(client.is_port_legal(MAX_PORT+1));
}

TEST(CClientInfoTest, is_port_free) {
    CClientInfo c;
    CClientInfoUT client(&c);
    client.m_bitmap_port_set_bit(2000);
    EXPECT_FALSE(client.is_port_free(2000));
    client.m_bitmap_port_reset_bit(2000);
    EXPECT_TRUE(client.is_port_free(2000));
}



TEST(CClientInfoTest,get_next_free_port_by_bit) {
    CClientInfo c;
    CClientInfoUT client(&c);
    client.m_head_port_set(200);
    client.get_next_free_port_by_bit();
    EXPECT_EQ(MIN_PORT, client.m_head_port_get());
    for(int idx=1024;idx<2000;idx++) {
        client.m_bitmap_port_set_bit(idx);
    }
    client.get_next_free_port_by_bit();
    EXPECT_EQ(1044, client.m_head_port_get());
}

TEST(CClientInfoTest, get_new_free_port) {
    CClientInfo c;
    CClientInfoUT client(&c);

    EXPECT_EQ(1024, client.obj->get_new_free_port());
    EXPECT_EQ(PORT_IN_USE, client.m_bitmap_port_get_bit(1024));

    client.m_bitmap_port_reset_bit(1024);
    EXPECT_EQ(PORT_FREE, client.m_bitmap_port_get_bit(1024));
    client.m_head_port_set(MAX_PORT-1);
    client.m_bitmap_port_set_bit(MAX_PORT-1);

    EXPECT_EQ(1024, client.obj->get_new_free_port());
    EXPECT_EQ(PORT_IN_USE, client.m_bitmap_port_get_bit(1024));
    client.m_head_port_set(1024);
    EXPECT_EQ(1025, client.obj->get_new_free_port());
    EXPECT_EQ(PORT_IN_USE, client.m_bitmap_port_get_bit(1025));

    for(int i=1024;i<1200;i++) {
        client.m_bitmap_port_set_bit(i);
    }
    client.m_head_port_set(1024);
    EXPECT_EQ(ILLEGAL_PORT, client.obj->get_new_free_port());

}


TEST(CClientInfoTest, return_port) {
    CClientInfo c;
    CClientInfoUT client(&c);
    client.m_bitmap_port_set_bit(2000);
    client.obj->return_port(2000);
    EXPECT_EQ(PORT_FREE, client.m_bitmap_port_get_bit(2000));
}

TEST(CClientInfoLTest, get_new_free_port) {
    CClientInfoL c;
    for(int i=0;i<10;i++) {
        EXPECT_EQ(1024+i, c.get_new_free_port());
    }
    c.return_all_ports();
    for(int i=0;i<10;i++) {
        EXPECT_EQ(1024+i, c.get_new_free_port());
    }
}



/* UIT of CTupleGeneratorSmart */
TEST(tuple_gen,GenerateTuple) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001,  0x10000f01, 0x30000001, 0x40000001, 
               MAX_PORT, MAX_PORT);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;

    for(int i=0;i<10;i++) {
        gen.GenerateTuple(result);
        printf(" C:%x S:%x P:%d \n",result.getClient(),result.getServer(),result.getClientPort());

        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i));
        EXPECT_EQ(result_dest, (uint32_t) (((0x30000001+i)) ) );
        EXPECT_EQ(result_port, 1024);
    }

    gen.Delete();
//    EXPECT_EQ((size_t)0, gen.m_clients.size());
}

TEST(tuple_gen,GenerateTuple2) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001, 
               MAX_PORT, MAX_PORT);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;

    for(int i=0;i<200;i++) {
        gen.GenerateTuple(result);
      //  gen.Dump(stdout);
      //  fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t)((0x30000001+i) ) );
        EXPECT_EQ(result_port, 1024+i/15);
    }

    gen.Delete();
//    EXPECT_EQ((size_t)0, gen.m_clients.size());
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001, 
               MAX_PORT,MAX_PORT);
    for(int i=0;i<200;i++) {
        gen.GenerateTuple(result);
    //    gen.Dump(stdout);
     //   fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t) (((0x30000001+i)) ) );
        EXPECT_EQ(result_port, 1024+i/15);
    }



}

TEST(tuple_gen,GenerateTupleMac) {
    CFlowGenList  fl;
    fl.Create();
    fl.load_from_mac_file("avl/mac_uit.yaml");
    fl.m_yaml_info.m_tuple_gen.m_clients_ip_start = 0x10000001;
    fl.m_yaml_info.m_tuple_gen.m_clients_ip_end = 0x1000000f;


    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001,  0x1000000f, 0x30000001, 0x40000001, 
               MAX_PORT, MAX_PORT, &fl);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;

    for(int i=0;i<10;i++) {
        gen.GenerateTuple(result);
        printf(" C:%x S:%x P:%d \n",result.getClient(),result.getServer(),result.getClientPort());

        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%2));
        EXPECT_EQ(result_dest, (uint32_t) (((0x30000001+i)) ) );
        EXPECT_EQ(result_port, 1024+i/2);
    }

    gen.Delete();
//    EXPECT_EQ((size_t)0, gen.m_clients.size());
}



TEST(tuple_gen,GenerateTupleEx) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               MAX_PORT, MAX_PORT);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;
    uint16_t ex_port[2];
    for(int i=0;i<20;i++) {

        gen.GenerateTupleEx(result,2,ex_port);
        fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();

        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t)(((0x30000001+i)) ));

        EXPECT_EQ(result_port, 1024+(i/15)*3);
        EXPECT_EQ(ex_port[0], 1025+(i/15)*3);
        EXPECT_EQ(ex_port[1], 1026+(i/15)*3);
    }

    gen.Delete();
}

TEST(tuple_gen,split1) {
    CClientPortion  portion;

    CTupleGenYamlInfo fi;
    fi.m_clients_ip_start =0x10000000;
    fi.m_clients_ip_end   =0x100000ff;

    fi.m_servers_ip_start =0x20000000;
    fi.m_servers_ip_end   =0x200000ff;

    fi.m_dual_interface_mask =0x01000000;

    split_clients(0,
                  1, 
                  0,
                  fi,
                  portion);
    EXPECT_EQ(portion.m_client_start, (uint32_t)(0x10000000));
    EXPECT_EQ(portion.m_client_end,   (uint32_t)(0x100000ff ));
    EXPECT_EQ(portion.m_server_start  , (uint32_t)(0x20000000));
    EXPECT_EQ(portion.m_server_end    , (uint32_t)(0x200000ff));
    printf(" %x %x %x %x \n",portion.m_client_start,portion.m_client_end,portion.m_server_start,portion.m_server_end);

    split_clients(2,
                  4, 
                  1,
                  fi,
                  portion);

     EXPECT_EQ(portion.m_client_start, (uint32_t)(0x11000080));
     EXPECT_EQ(portion.m_client_end, (uint32_t)(0x110000bf ));
     EXPECT_EQ(portion.m_server_start  , (uint32_t)(0x21000080));
     EXPECT_EQ(portion.m_server_end    , (uint32_t)(0x210000bf));
     printf(" %x %x %x %x \n",portion.m_client_start,portion.m_client_end,portion.m_server_start,portion.m_server_end);
}

TEST(tuple_gen,split2) {
    CClientPortion  portion;

    CTupleGenYamlInfo fi;
    fi.m_clients_ip_start =0x10000000;
    fi.m_clients_ip_end   =0x100001ff;

    fi.m_servers_ip_start =0x20000000;
    fi.m_servers_ip_end   =0x200001ff;

    fi.m_dual_interface_mask =0x01000000;

    int i;
    for (i=0; i<8; i++) {
        split_clients(i,
                      8, 
                      (i&1),
                      fi,
                      portion);


        if ( (i&1) ) {
            EXPECT_EQ(portion.m_client_start, (uint32_t)(0x11000000)+(0x40*i));
            EXPECT_EQ(portion.m_client_end, (uint32_t)(0x11000000 +(0x40*i+0x40-1)));
            EXPECT_EQ(portion.m_server_start  , (uint32_t)(0x21000000)+ (0x40*i) );
            EXPECT_EQ(portion.m_server_end    , (uint32_t)(0x21000000)+(0x40*i+0x40-1) );
        }else{
            EXPECT_EQ(portion.m_client_start, (uint32_t)(0x10000000)+ (0x40*i) );
            EXPECT_EQ(portion.m_client_end, (uint32_t)(0x10000000   + (0x40*i+0x40-1) ) );
            EXPECT_EQ(portion.m_server_start  , (uint32_t)(0x20000000) + (0x40*i) );
            EXPECT_EQ(portion.m_server_end    , (uint32_t)(0x20000000) + (0x40*i+0x40-1) );
        }
        printf(" %x %x %x %x \n",portion.m_client_start,portion.m_client_end,portion.m_server_start,portion.m_server_end);
    }
}





TEST(tuple_gen,template1) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               MAX_PORT, MAX_PORT);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);
    template_1.SetSingleServer(true,0x12121212,0,0);
    CTupleBase result;


    int i;
    for (i=0; i<10; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
        //printf(" %x %x %x \n",result_src,result_dest,result_port);
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i));
        EXPECT_EQ(result_dest, (uint32_t)(((0x12121212)) ));
    }

    template_1.Delete();
    gen.Delete();
}

TEST(tuple_gen,template2) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               MAX_PORT, MAX_PORT);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);
    template_1.SetW(10);

    CTupleBase result;


    int i;
    for (i=0; i<20; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
        //printf(" %x %x %x \n",result_src,result_dest,result_port);
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+(i/10)));
        EXPECT_EQ(result_dest, (uint32_t)(((0x30000001+ (i/10) )) ));
        EXPECT_EQ(result_port,   1024+(i%10));
    }

    template_1.Delete();
    gen.Delete();
}


TEST(tuple_gen,no_free) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x10000001, 0x30000001, 0x300000ff,
               MAX_PORT, MAX_PORT);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);

    CTupleBase result;


    int i;
    for (i=0; i<65557; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
    }
    // should have error
    EXPECT_TRUE((gen.getErrorAllocationCounter()>0)?true:false);

    template_1.Delete();
    gen.Delete();
}

TEST(tuple_gen,try_to_free) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x10000001, 0x30000001, 0x300000ff,
               MAX_PORT, MAX_PORT);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);

    CTupleBase result;


    int i;
    for (i=0; i<65557; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
        gen.FreePort(result_src,result_port);
    }
    // should have error
    EXPECT_FALSE((gen.getErrorAllocationCounter()>0)?true:false);

    template_1.Delete();
    gen.Delete();
}



/* tuple generator using CClientInfoL*/
TEST(tuple_gen_2,GenerateTuple) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001,  0x10000f01, 0x30000001, 0x40000001, 
               0,0);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;

    for(int i=0;i<10;i++) {
        gen.GenerateTuple(result);
        printf(" C:%x S:%x P:%d \n",result.getClient(),result.getServer(),result.getClientPort());

        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i));
        EXPECT_EQ(result_dest, (uint32_t) (((0x30000001+i)) ) );
        EXPECT_EQ(result_port, 1024);
    }

    gen.Delete();
//    EXPECT_EQ((size_t)0, gen.m_clients.size());
}

TEST(tuple_gen_2,GenerateTuple2) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001, 
               0,0);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;

    for(int i=0;i<200;i++) {
        gen.GenerateTuple(result);
      //  gen.Dump(stdout);
      //  fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t)((0x30000001+i) ) );
        EXPECT_EQ(result_port, 1024+i/15);
    }

    gen.Delete();
//    EXPECT_EQ((size_t)0, gen.m_clients.size());
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001, 
               0,0);
    for(int i=0;i<200;i++) {
        gen.GenerateTuple(result);
    //    gen.Dump(stdout);
     //   fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t) (((0x30000001+i)) ) );
        EXPECT_EQ(result_port, 1024+i/15);
    }



}



TEST(tuple_gen_2,GenerateTupleEx) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               0,0);
    CTupleBase result;
    uint32_t result_src;
    uint32_t result_dest;
    uint16_t result_port;
    uint16_t ex_port[2];
    for(int i=0;i<20;i++) {

        gen.GenerateTupleEx(result,2,ex_port);
        fprintf(stdout, "i:%d\n",i);
        result_src = result.getClient();
        result_dest = result.getServer();
        result_port = result.getClientPort();

        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i%15));
        EXPECT_EQ(result_dest, (uint32_t)(((0x30000001+i)) ));

        EXPECT_EQ(result_port, 1024+(i/15)*3);
        EXPECT_EQ(ex_port[0], 1025+(i/15)*3);
        EXPECT_EQ(ex_port[1], 1026+(i/15)*3);
    }

    gen.Delete();
}

TEST(tuple_gen_2,template1) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               0,0);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);
    template_1.SetSingleServer(true,0x12121212,0,0);
    CTupleBase result;


    int i;
    for (i=0; i<10; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
        //printf(" %x %x %x \n",result_src,result_dest,result_port);
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+i));
        EXPECT_EQ(result_dest, (uint32_t)(((0x12121212)) ));
        EXPECT_EQ(result_port, 1024);
    }

    template_1.Delete();
    gen.Delete();
}

TEST(tuple_gen_2,template2) {
    CTupleGeneratorSmart gen;
    gen.Create(1, 1,cdSEQ_DIST, 
               0x10000001, 0x1000000f, 0x30000001, 0x40000001,
               0,0);
    CTupleTemplateGeneratorSmart template_1;
    template_1.Create(&gen);
    template_1.SetW(10);

    CTupleBase result;


    int i;
    for (i=0; i<20; i++) {
        template_1.GenerateTuple(result);
        uint32_t result_src = result.getClient();
        uint32_t result_dest = result.getServer();
        uint16_t result_port = result.getClientPort();
        //printf(" %x %x %x \n",result_src,result_dest,result_port);
        EXPECT_EQ(result_src, (uint32_t)(0x10000001+(i/10)));
        EXPECT_EQ(result_dest, (uint32_t)(((0x30000001+ (i/10) )) ));
        EXPECT_EQ(result_port,   1024+(i%10));
    }

    template_1.Delete();
    gen.Delete();
}


TEST(tuple_gen_yaml,yam_reader1) {

    CTupleGenYamlInfo  fi;

    try {
       std::ifstream fin((char *)"cap2/tuple_gen.yaml");
       YAML::Parser parser(fin);
       YAML::Node doc;

       parser.GetNextDocument(doc);
       for(unsigned i=0;i<doc.size();i++) {
          doc[i] >> fi;
          break;
       }
    } catch ( const std::exception& e ) {
        std::cout << e.what() << "\n";
        exit(-1);
    }
    fi.Dump(stdout);
}

TEST(tuple_gen_yaml,yam_is_valid) {

    CTupleGenYamlInfo  fi;

    fi.m_clients_ip_start = 0x10000001;
    fi.m_clients_ip_end   = 0x100000ff;

    fi.m_servers_ip_start = 0x10000001;
    fi.m_servers_ip_end   = 0x100001ff;

    EXPECT_EQ(fi.is_valid(8,true)?1:0, 1);
    EXPECT_EQ(fi.m_servers_ip_start, 0x10000001);
    EXPECT_EQ(fi.m_servers_ip_end, 0x100001fe);

    printf(" start:%x end:%x \n",fi.m_servers_ip_start,fi.m_servers_ip_end);

    fi.m_clients_ip_start = 0x10000001;
    fi.m_clients_ip_end   = 0x100000ff;

    fi.m_servers_ip_start = 0x10000001;
    fi.m_servers_ip_end   = 0x10000009;

    EXPECT_EQ(fi.is_valid(8,true)?1:0, 0);

    fi.m_clients_ip_start = 0x10000001;
    fi.m_clients_ip_end   = 0x100000ff;

    fi.m_servers_ip_start = 0x10000001;
    fi.m_servers_ip_end   = 0x100003ff;

    EXPECT_EQ(fi.is_valid(8,true)?1:0, 1);
    EXPECT_EQ(fi.m_servers_ip_start, 0x10000001);
    EXPECT_EQ(fi.m_servers_ip_end, 0x100003fc);

    printf(" start:%x end:%x \n",fi.m_servers_ip_start,fi.m_servers_ip_end);


}







/*GTEST_API_ int main(int argc, char **argv) {
      testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
} */
