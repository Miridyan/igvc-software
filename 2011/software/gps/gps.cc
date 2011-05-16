#include "gps.hpp"

gps::gps() : running(false), gps_port(io_service)
{
	
}
gps::~gps()
{
	stop();
}

void gps::start()
{
	running = true;
	iothread = boost::thread(boost::bind(&gps::gps_comm, this));
}

void gps::stop()
{
	running = false;
	iothread.join();
}

const gps::GPSState& gps::get_last_pos()
{
	boost::mutex::scoped_lock lock(state_queue_mutex);
	return state_queue.back();
}

bool gps::open(char* port, size_t baud)
{
	gps_port.open(port);

	if(!gps_port.is_open())
	{
		return false;
	}
	
	gps_port.set_option(boost::asio::serial_port_base::baud_rate(baud));
	//gps_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	gps_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	gps_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

	return true;
}

void gps::gps_comm()
{
	while(running)
	{
		boost::asio::read_until(gps_port, comm_buffer, '\n');
		std::istream is(&comm_buffer);
		std::string line;
		std::getline(is, line);

		GPSState state;
		bool ret = parse_message(line, state);
	}
}

bool gps::parse_message(const std::string& line, GPSState& state)
{
	char message[1024];
	strcpy(message, line.c_str());
	
	char * pch;
	pch = strtok (message,",");
	if(strncmp("GPGLL",pch,5)==0)
	{
		//printf("got lat:");
		pch = strtok (NULL, ",");
		//printf ("messlat: %s\n",pch);
		state.lat = atof(pch);
		//printf("storelat:%f\n",latlon.lat);
		pch = strtok (NULL, ",");
//		latlon.lat_dir = * pch;
		pch = strtok (NULL, ",");
		//printf ("messlon: %s\n",pch);
		state.lon = atof(pch);
		pch = strtok (NULL, ",");
//		latlon.lon_dir = *pch;
		//printf("storelon:%f\n",latlon.lon);
	}
	while(pch!=NULL)
	{
		//printf("doing some more tokens\n");
		pch = strtok (NULL, ",");
	}
}