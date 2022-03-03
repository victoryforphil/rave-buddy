#include "data/uuids.h"

const char* UUIDS[] = {
"cfd30431-362b-462e-be80-42fd8541b344",
"0162f9be-1cf9-4daf-8f3b-172476dfd008",
"a0b655b4-7cfc-4952-8676-4a223820bfd1",
"a09fd7e0-cfe0-48dc-b45e-accaeb2a76eb",
"a76a1dcb-ebec-469d-9cfe-bc94fe62a455",
"663debd9-b600-486a-a8b0-0c4bd107a4ce",
"194be51f-b33d-4814-8ca5-78b0ad40b22b",
"b0257ed0-972c-4880-9ae8-f42ea6e93b22",
"6bbcade9-15f3-4011-b232-114c9abb3024",
"3f18faee-8b57-4c62-8ff2-ed172928784c",
"df154ff7-0029-4d71-9fa7-06484b0c68ec",
"d316f952-46c7-4bf6-b767-d8f228be2d08",
"ae106b52-a02e-48ee-b118-6ce12103659e",
"0871ce45-8010-4cbb-8150-1567978e3f89",
"2706db00-72e3-46c5-b2e2-7f10a76d118d",
"beb42028-8527-4a3f-9dac-d3c9072b9218",
"12153b79-d356-4ff1-8d25-5aeb66793369",
"a18fb11c-25a8-45cf-9661-0f984c0a45fb",
"6211882a-5f33-419f-952e-17a0cab050b8",
"05e7ae79-e423-4248-b427-85adebf8241f",
"8d192470-4680-48a8-ab9b-d37c88f3d428",
"4488dc2f-3ef8-4119-9175-7df0298eb8ce",
"583df24f-426e-47c3-93a1-8331bad0c252",
"2c35ab8a-cf39-43f5-922c-0efbdd727f60",
"71c3d833-5494-4c36-aa04-d5f7c8a07fd3",
"5db7f1c4-ca4d-48d0-b77a-961ced77e14f",
"d5e29d1f-bec7-4491-8acd-0110f30c8bd3",
"aad994f7-94a8-45b8-8597-528470e62ef6",
"fa2427f8-50a2-4355-90b0-dc00a751457f",
"cba02aa8-41cc-4020-84a4-f6ffc9c86592",
"b4f59f3a-c68c-41ad-af7a-cfd8fb4c173b",
"f1214dd5-7e7c-446d-a7b0-61a3d3611161",
"e320a1be-5e09-4415-9061-e3549bad4139",
"cbc284a9-732e-421f-8a50-4762cb8f9931",
"8aa9f417-9760-4c61-b875-b3cccc906254",
"db0b25ad-39c4-420a-aad7-e1024717d9fb",
"2299ba0e-efd9-4527-a82e-d518e0edc5c1",
"e3bcdead-0418-427e-bece-cd7a10a4de21",
"469b14d5-4885-4b4f-a526-9af19a517580",
"eed04aa6-f922-4197-80b5-4315139f9d66",
"ab75f654-fc68-4f5e-bd8d-303bd7ff2f60",
"58a0952b-6adb-4874-839c-6f57aa47cbe2",
"f139a509-5a40-4f08-bc5f-200f14884d20",
"98eb8842-e79d-4d91-8a95-bbf52baeb3c6",
"3e8acdfc-a7df-42e4-8471-615513815c2b",
"50535114-71b0-4141-9a57-44fe68647d37",
"b1004c4d-4a32-48bd-8181-07919431e80f",
"63ff1738-6b84-46ca-a810-9474e1f19480",
"2f04c493-8a67-4026-9bda-995898ebb4d3",
"7a43f6d5-b016-4bc0-88fe-c29a4ba64352",
"086de0b4-516e-4ca7-909b-72176951dc8d",
"3cac3c03-c2c4-4023-a030-37103d3800cd",
"a67df3cf-f9a5-4c88-86ca-7459e8991808",
"36bcd75d-1f46-4699-ab4d-7214e823fb19",
"9073ee9e-58a7-4605-8fe0-ff8978c6d8b9",
"f595aed7-2ce0-46c0-99e9-6d065949f7c7",
"2106b233-d879-4c60-89c7-e00019d4e98e",
"df6dbe68-4846-4a08-9a38-78138e24bd7d",
"0adbd6f1-5806-4619-82f6-e989573f8571",
"936e1cb3-afe6-4db8-bb83-9309c4c2676c",
"10c23e9d-5d89-4d20-8330-11ab51e2482b",
"cdae4c4d-8487-472f-bfa5-4ca35d553fbf",
"aa1a84bf-4da5-4b1a-8492-4cc9c63f6e58",
"d0887056-c3d4-429b-a550-664cff52b1f2",
"d126f6c1-f5ae-408d-87ff-6c61eee3d15a",
"038e6524-5e9a-4e75-a6cb-515567a79749",
"bc0ad6a1-30bc-438f-ba33-e0a0d4fe5068",
"1ead27d7-d0ed-4fe5-a8cd-044e30c99cd6",
"9f686fd0-f5d7-452b-9f96-53c2d8a717ce",
"af5ba669-76ad-43ef-a4b8-ed1b5c4b8977",
"f240a441-1fe0-40ab-b59f-d2c4883c7e16",
"c3663225-b24d-4e14-805c-5307e819b7b4",
"e555fce7-d1af-4ba7-b393-f1da271ec41d",
};

int _uuid_idx;
const char* uuid_get(){
    return UUIDS[_uuid_idx++];
}