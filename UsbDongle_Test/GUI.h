#pragma once

#include <msclr/marshal_cppstd.h>
#include <iostream>

#include "libusb.h"
#include "usb_ctl.h"

static uint8_t reset_table[] = {
	0xD2, 0x10,  // sleep mode
	0xD1, 0x80   // soft reset, delay must put here
};

static uint8_t regs_table[] = {
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12   // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
};

static uint8_t init_table_301c[] = {
	0xD2, 0x10,
	0x30, 0x0A,
	0x31, 0x0A,
	0x32, 0x07,
	0x35, 0x08,
	0x36, 0x8C,
	0x37, 0x64,
	0x38, 0x07,
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12,  // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
	0x3A, 0x80,
	0x3B, 0x02,
	0xD2, 0x13,
	0xD3, 0x00   // clear status, disable DVR, VRB calibration
};

namespace UsbDongle_Test {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Text;
	using namespace System::IO;

	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;

	/// <summary>
	/// GUI 的摘要
	/// </summary>
	public ref class GUI : public System::Windows::Forms::Form
	{
	public:
		GUI(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//

			pb_image->Image = imageList1->Images[0];
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~GUI()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnOpen_device;
	private: System::Windows::Forms::Button^  btnRealTime;
	private: System::Windows::Forms::Button^  btnWrite_Reg;
	private: System::Windows::Forms::Button^  btnRead_Reg;

	protected:

	private: System::Windows::Forms::Label^  lb_info;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::PictureBox^  pb_image;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Timer^  realtime_timer;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  tb_value;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  tb_register;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ImageList^  imageList1;

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		Bitmap^ logo_bmp;
		struct libusb_device_handle *dev_handle = NULL;

		MemoryStream^ memStream;
		uint8_t frame_toggle = 0x02;
		array<Byte>^ byteArray;

		uint16_t	pixel_cnt = 0;
		int			transfer_cnt = 0;

		uint8_t	   *test_buf;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GUI::typeid));
			this->btnOpen_device = (gcnew System::Windows::Forms::Button());
			this->btnRealTime = (gcnew System::Windows::Forms::Button());
			this->btnWrite_Reg = (gcnew System::Windows::Forms::Button());
			this->btnRead_Reg = (gcnew System::Windows::Forms::Button());
			this->lb_info = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->pb_image = (gcnew System::Windows::Forms::PictureBox());
			this->realtime_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->tb_value = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tb_register = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// btnOpen_device
			// 
			this->btnOpen_device->BackColor = System::Drawing::SystemColors::Control;
			this->btnOpen_device->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnOpen_device->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnOpen_device->Location = System::Drawing::Point(211, 141);
			this->btnOpen_device->Name = L"btnOpen_device";
			this->btnOpen_device->Size = System::Drawing::Size(125, 40);
			this->btnOpen_device->TabIndex = 9;
			this->btnOpen_device->Text = L"Open Device";
			this->btnOpen_device->UseVisualStyleBackColor = true;
			this->btnOpen_device->Click += gcnew System::EventHandler(this, &GUI::btnOpen_device_Click);
			// 
			// btnRealTime
			// 
			this->btnRealTime->BackColor = System::Drawing::SystemColors::Control;
			this->btnRealTime->Enabled = false;
			this->btnRealTime->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRealTime->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRealTime->Location = System::Drawing::Point(211, 200);
			this->btnRealTime->Name = L"btnRealTime";
			this->btnRealTime->Size = System::Drawing::Size(125, 40);
			this->btnRealTime->TabIndex = 10;
			this->btnRealTime->Text = L"RealTime";
			this->btnRealTime->UseVisualStyleBackColor = true;
			this->btnRealTime->Click += gcnew System::EventHandler(this, &GUI::btnRealTime_Click);
			// 
			// btnWrite_Reg
			// 
			this->btnWrite_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnWrite_Reg->Enabled = false;
			this->btnWrite_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnWrite_Reg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnWrite_Reg->Location = System::Drawing::Point(347, 141);
			this->btnWrite_Reg->Name = L"btnWrite_Reg";
			this->btnWrite_Reg->Size = System::Drawing::Size(125, 40);
			this->btnWrite_Reg->TabIndex = 11;
			this->btnWrite_Reg->Text = L"Write Register";
			this->btnWrite_Reg->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->btnWrite_Reg->UseVisualStyleBackColor = true;
			this->btnWrite_Reg->Click += gcnew System::EventHandler(this, &GUI::btnWrite_Reg_Click);
			// 
			// btnRead_Reg
			// 
			this->btnRead_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnRead_Reg->Enabled = false;
			this->btnRead_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRead_Reg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRead_Reg->Location = System::Drawing::Point(347, 200);
			this->btnRead_Reg->Name = L"btnRead_Reg";
			this->btnRead_Reg->Size = System::Drawing::Size(125, 40);
			this->btnRead_Reg->TabIndex = 12;
			this->btnRead_Reg->Text = L"Read Register";
			this->btnRead_Reg->UseVisualStyleBackColor = true;
			this->btnRead_Reg->Click += gcnew System::EventHandler(this, &GUI::btnRead_Register_Click);
			// 
			// lb_info
			// 
			this->lb_info->BackColor = System::Drawing::Color::White;
			this->lb_info->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lb_info->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_info->Location = System::Drawing::Point(210, 15);
			this->lb_info->Name = L"lb_info";
			this->lb_info->Size = System::Drawing::Size(470, 87);
			this->lb_info->TabIndex = 13;
			this->lb_info->Text = L"Welcome use usb dongle test ap";
			this->lb_info->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->pb_image);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->groupBox2->Location = System::Drawing::Point(7, 8);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(197, 214);
			this->groupBox2->TabIndex = 15;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Show Image";
			// 
			// pb_image
			// 
			this->pb_image->Location = System::Drawing::Point(8, 25);
			this->pb_image->Name = L"pb_image";
			this->pb_image->Size = System::Drawing::Size(180, 180);
			this->pb_image->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_image->TabIndex = 0;
			this->pb_image->TabStop = false;
			// 
			// realtime_timer
			// 
			this->realtime_timer->Tick += gcnew System::EventHandler(this, &GUI::realtime_timer_Tick);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->tb_value);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->tb_register);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(483, 114);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(197, 128);
			this->groupBox1->TabIndex = 16;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Register Parameter";
			// 
			// tb_value
			// 
			this->tb_value->Enabled = false;
			this->tb_value->Location = System::Drawing::Point(123, 83);
			this->tb_value->MaxLength = 2;
			this->tb_value->Name = L"tb_value";
			this->tb_value->Size = System::Drawing::Size(68, 24);
			this->tb_value->TabIndex = 4;
			this->tb_value->Text = L"00";
			this->tb_value->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(122, 86);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(0, 18);
			this->label3->TabIndex = 3;
			// 
			// tb_register
			// 
			this->tb_register->Enabled = false;
			this->tb_register->Location = System::Drawing::Point(123, 41);
			this->tb_register->MaxLength = 2;
			this->tb_register->Name = L"tb_register";
			this->tb_register->Size = System::Drawing::Size(68, 24);
			this->tb_register->TabIndex = 2;
			this->tb_register->Text = L"00";
			this->tb_register->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(7, 86);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(70, 20);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Value    :";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(7, 43);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(77, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Register :";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(218, 112);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(181, 18);
			this->label4->TabIndex = 17;
			this->label4->Text = L"VID : 0x06cb, PID : 0x0040";
			// 
			// imageList1
			// 
			this->imageList1->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^>(resources->GetObject(L"imageList1.ImageStream")));
			this->imageList1->TransparentColor = System::Drawing::Color::Transparent;
			this->imageList1->Images->SetKeyName(0, L"logo.png");
			// 
			// GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(692, 254);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->btnRead_Reg);
			this->Controls->Add(this->btnWrite_Reg);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->lb_info);
			this->Controls->Add(this->btnRealTime);
			this->Controls->Add(this->btnOpen_device);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"UsbDongle_test_tool (libusb)";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUI::GUI_FormClosed);
			this->Load += gcnew System::EventHandler(this, &GUI::GUI_Load);
			this->groupBox2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion

	private: System::Void GUI_Load(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;

				 res = libusb_init(NULL);
				 if (res < 0)
					 printf("libusb init fail.\n");
				 else
					 printf("libusb init success.\n");
	}

	private: System::Void btnOpen_device_Click(System::Object^  sender, System::EventArgs^  e) {
				 int Rst = -1;

				 Rst = usb_open_device(0x06cb, 0x0040);
				 if (Rst < 0) {
					 printf("usb_open_device fail. (%d)\n", Rst);
					 lb_info->ForeColor = Color::Red;
					 lb_info->Text = "Device not found,\n please connect device.";
				 }
				 else {
					 Rst = usb_write_register(reset_table, ARRAYSIZE(reset_table));
					 if (Rst > 0) {
						 Rst = usb_write_register(init_table_301c, ARRAYSIZE(init_table_301c));
						 if (Rst > 0) {
							 lb_info->ForeColor = Color::Black;
							 lb_info->Text = "Open device success.";

							 switch_button_state(true);
							 btnOpen_device->Enabled = false;
						 }
					 }
				 }

				 dev_handle = get_dev_handle();
	}

	private: System::Void btnRealTime_Click(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;

				 if (btnRealTime->Text == "RealTime") {
					 btnRealTime->Enabled = true;
					 btnRealTime->Text = "Stop";

					 res = usb_write_register(init_table_301c, ARRAYSIZE(init_table_301c));
					 if (res < 0) {
						 printf("Init sensor fail.\n");

						 lb_info->ForeColor = Color::Red;
						 lb_info->Text = "Device not found,\n please connect device.";
						 switch_button_state(false);
						 btnOpen_device->Enabled = true;
					 }
					 else {
						 printf("Init sensor success. (%d)\n", res);

						 res = usb_start_stream();
						 if (res < 0) {
							 printf("usb_sensor_start_stream fail. (%d)\n", res);
						 }
						 else {
							 printf("Press finger to show real-time image on device\n");
							 lb_info->ForeColor = Color::Black;
							 lb_info->Text = "Press finger to show real-time\n image on device.";
							 lb_info->Refresh();

							 switch_button_state(false);
							 btnRealTime->Enabled = true;

							 realtime_timer->Interval = 33;
							 realtime_timer->Start();

							 test_buf = new uint8_t[FRAME_SIZE];

							 memStream = gcnew MemoryStream();
						 }
					 }
				 }
				 else {
					 res = usb_stop_stream();
					 if (res < 0) {
						 printf("usb_sensor_stop_stream fail. (%d)\n", res);
					 }
					 else {
						 realtime_timer->Stop();

						 lb_info->ForeColor = Color::Black;
						 lb_info->Text = "Welcome use usb dongle test ap";
						 switch_button_state(true);
						 btnOpen_device->Enabled = false;

						 btnRealTime->Text = "RealTime";
					 }
				 }
	}

	private: System::Void realtime_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;

				 uint8_t data[TRANSFER_SIZE];
				 uint8_t convert_data[TRANSFER_SIZE];
				 uint8_t new_data[20000];
				 
				 int transferred;
				 int tag = 0;
				 int dat_len;

				 int hdr_idx = 0;
				 int dat_idx = 1;

				 uint8_t* img_data;
				 
				 /*
				 res = usb_get_image_data(dev_handle, &img_data);			
				 
				 if (res == 1) {		
					 
					 for (int idx = 0; idx < FRAME_SIZE; idx++) {
						 img_data[idx] = 255 - img_data[idx];
					 }
					 
					 array<Byte>^ byteArray = gcnew array<Byte>(FRAME_SIZE);
					 Marshal::Copy((IntPtr)img_data, byteArray, 0, FRAME_SIZE);

					 pb_image->Image = ToGrayBitmap(byteArray, 128, 128);
				 }
				 */
				 			 
				 res = libusb_bulk_transfer(dev_handle, 0x81, data, TRANSFER_SIZE, &transferred, 0);
				 if (res < 0) {
					 printf("libusb_bulk_transfer error.(%d)\n", res);
				 }
				 else {		
					 /*
					 while (hdr_idx < transferred) {
						 if ((data[hdr_idx] & 0x02) != 0x02) {
							 if ((data[hdr_idx] & 0x01) != frame_toggle) {
								 //printf("Total transfer ----------------- pixel(%d) total pocket(%d)\n", pixel_cnt, transfer_cnt);		

								 for (int idx = 0; idx < FRAME_SIZE; idx++) {
									 test_buf[idx] = 0xFF - test_buf[idx];
								 }

								 array<Byte>^ byteArray = gcnew array<Byte>(FRAME_SIZE);
								 Marshal::Copy((IntPtr)test_buf, byteArray, 0, FRAME_SIZE);

								 pb_image->Image = ToGrayBitmap(byteArray, 128, 128);
								 

								 pixel_cnt = 0;
								 transfer_cnt = 0;

								 frame_toggle = (data[hdr_idx] & 0x01);
							 }

							 dat_len = transferred - dat_idx;
							 dat_len = (dat_len > PAYLOAD_SIZE ? PAYLOAD_SIZE : dat_len);

							 if ((pixel_cnt + dat_len) <= FRAME_SIZE) {
								 memcpy(&test_buf[pixel_cnt], &data[dat_idx], dat_len);
								 pixel_cnt += dat_len;
							 }

							 hdr_idx += 64;
							 dat_idx += 64;
						 }
						 else {
							 ++hdr_idx;
							 ++dat_len;
						 }
					 }
					 */
					 
					 if (transferred > 0) {
						 if ((data[0] & 0x02) != 0x02) {
							 if ((data[0] & 0x01) != frame_toggle) {

								 frame_toggle = (data[0] & 0x01);								

								 array<Byte>^ r = memStream->ToArray();
								 Console::WriteLine("MemoryStream Length = " + r->Length);

								 if (r->Length == 16384) {
									 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);

									 pb_image->Image = ToGrayBitmap(r, 128, 128);									
								 }
								 memStream = gcnew MemoryStream();
							 }

							 for (int y = 0; y < transferred; y++) {
								 if (y % 64 != 0) {
									 convert_data[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }

							 for (int i = 0; i < transferred - tag; i++) {
								 convert_data[i] = 255 - convert_data[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convert_data, byteArray, 0, transferred - tag);

							 memStream->Write(byteArray, 0, transferred - tag);
						 }
					 }				 
				 }					 
	}
	
	private: System::Void btnWrite_Reg_Click(System::Object^  sender, System::EventArgs^  e) {
				 int Rst = -1;
				 uint16_t u16_Reg;
				 uint16_t u16_Value;
				 unsigned char write_register[] = {0x00, 0x00};
			
				 if (tb_register->Text != "00" && tb_register->Text != "0" && tb_register->Text != "") {
					 String^ reg = String::Format("0x{0:X2}", tb_register->Text);
					 String^ value = String::Format("0x{0:X2}", tb_value->Text);

					 IntPtr convertReg = Marshal::StringToHGlobalAnsi(reg);
					 char* nativeCharReg = static_cast<char*>(convertReg.ToPointer());
					 u16_Reg = strtol(nativeCharReg, NULL, 16);
					 Console::WriteLine("write reg-> " + u16_Reg);

					 IntPtr convertValue = Marshal::StringToHGlobalAnsi(value);
					 char* nativeCharValue = static_cast<char*>(convertValue.ToPointer());
					 u16_Value = strtol(nativeCharValue, NULL, 16);
					 Console::WriteLine("write value-> " + u16_Value);

					 write_register[0] = u16_Reg;
					 write_register[1] = u16_Value;

					 Rst = usb_write_register(write_register, ARRAYSIZE(write_register));
					 if (Rst < 0) {
						 printf("usb_sensor_write_register error. (%d)\n", Rst);
						 lb_info->Text = "Write register fail.\n\n";
						 tb_register->Text = "00";
						 tb_value->Text = "00";
					 }
					 else {
						 lb_info->Text  = "Write register success.\n\n";
						 lb_info->Text += "Register address: 0x" + tb_register->Text + ",  Value : 0x" + tb_value->Text;
					 }
				 }
				 else {
					 lb_info->Text  = "Write register fail.\n\n";
					 lb_info->Text += "Register address can't be zero or null.";
					 tb_register->Text = "00";
					 tb_value->Text = "00";
				 }
	}

	private: System::Void btnRead_Register_Click(System::Object^  sender, System::EventArgs^  e) {
				 int Rst = -1;
				 uint8_t RegData;
				 uint16_t u16;

				 if (tb_register->Text != "00" && tb_register->Text != "0" && tb_register->Text != "") {
					 String^ reg = String::Format("0x00{0:X2}", tb_register->Text);

					 IntPtr convertString = Marshal::StringToHGlobalAnsi(reg);
					 char* nativeChar = static_cast<char*>(convertString.ToPointer());

					 u16 = strtol(nativeChar, NULL, 16);
					 Console::WriteLine("test -> " + u16);

					 Rst = usb_read_register(u16, &RegData);
					 if (Rst < 0) {
						 printf("usb_read_register error. (%d)", Rst);
						 lb_info->Text = "Read register fail.\n\n";
						 tb_value->Text = "00";
					 }
					 else {
						 tb_value->Text = "" + String::Format("{0:X2}", RegData);

						 lb_info->Text = "Read register success.\n\n";
						 lb_info->Text += "Register address: 0x" + tb_register->Text + ",  Value : " + String::Format("0x{0:X2}", RegData);
					 }
				 }
				 else {
					 lb_info->Text = "Read register fail.\n\n";
					 lb_info->Text += "Register address can't be zero or null.";
					 tb_register->Text = "00";
					 tb_value->Text = "00";
				 }
	}

	private: System::Void GUI_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 int res = -1;

				 res = usb_stop_stream();
				 if (res < 0) {
					 printf("usb_sensor_stop_stream fail. (%d)\n", res);
				 }
				 else {
					 printf("usb_sensor_stop_stream success. (%d)\n", res);
				 }

				 libusb_close(dev_handle);
	}

	public: static Bitmap^ ToGrayBitmap(array<Byte>^ rawValues, int width, int height) {
				Bitmap^ bmp = gcnew Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
				BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);

				int stride = bmpData->Stride;
				int offset = stride - width;
				IntPtr iptr = bmpData->Scan0;
				int scanBytes = stride * height;

				int posScan = 0, posReal = 0;

				array<Byte>^ pixelValues = gcnew array<Byte>(scanBytes);

				for (int x = 0; x < height; x++) {
					for (int y = 0; y < width; y++) {
						pixelValues[posScan++] = rawValues[posReal++];
					}
					posScan += offset;
				}

				Marshal::Copy(pixelValues, 0, iptr, scanBytes);
				bmp->UnlockBits(bmpData);


				ColorPalette^ temPalette;

				Bitmap^ tempBmp = gcnew Bitmap(1, 1, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				temPalette = tempBmp->Palette;

				for (int i = 0; i < 256; i++) {
					temPalette->Entries[i] = Color::FromArgb(i, i, i);
				}

				bmp->Palette = temPalette;

				return bmp;
	}

	public: Void switch_button_state(boolean flag)
	{
					btnOpen_device->Enabled = flag;
					btnRealTime->Enabled = flag;
					btnRead_Reg->Enabled = flag;
					btnWrite_Reg->Enabled = flag;

					tb_register->Enabled = flag;
					tb_value->Enabled = flag;
	}

};
}
