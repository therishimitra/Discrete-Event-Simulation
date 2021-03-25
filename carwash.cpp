
// carwash.cpp: implementation of the car wash simulator

#include <random>  // needs c++11
#include <queue>
#include <list>
#include "customer.h"
#include "washevent.h"

#define RAND1 double(rand()) / RAND_MAX

bool read_debug_option();

int main()
{
    // average time interval between two consecutive arrivals
	double ave_interarrival_time;

    // average service time for each customer
	double ave_service_time;

    // total simulation time
	double total_simulation_time;

    // debug options
	bool debug_opt1, debug_opt2;

	//----------------------------------------------------------------------
	//get user inputs
	cout << "Input the average time interval of two consecutive arrivals: ";
	cin >> ave_interarrival_time;

	cout << "Input the average service time: ";
	cin >> ave_service_time;

	cout << "Input the simulation time: ";
	cin >> total_simulation_time;

	// The following can be comment out during debugging
	// total simulation time should be at least 10,000 * ave_interarrival_time
	if (total_simulation_time < 10 * ave_interarrival_time)
	{
		cout << "Error: total simulation time should be greater than " 
			<< ave_interarrival_time * 10000 << endl;
		return 1;
	}

	cout << "Debugging option:" << "\n";
    cout << "- Print items in queue whenever there is a change in queue? (y/n): ";
	debug_opt1 = read_debug_option();
	cout << "- Print events when there is an arrival or departure? (y/n): ";
	debug_opt2 = read_debug_option();

	//----------------------------------------------------------------
	// create a queue of customers coming to the carwash
    queue<customer*> line;

	//create a list of events
	list<washevent*> event_list;

	//initialization randon number generator and distributions
	random_device rd;
	default_random_engine gen(rd());
	uniform_real_distribution<double> arrival_dis(0.0, 2.0*ave_interarrival_time);
	uniform_real_distribution<double> service_dis(0.0, 2.0*ave_service_time);
	
	//-----------------------------------------------------------
	// TODO: 
	// 1. generate the first arrival, push into event_list
   	// 2. generate an "end_of_simulation" event, push into event_list
	unsigned int id=1;
	
	double arrival_time = arrival_dis(gen);
	//double service_time = service_dis(gen);

	washevent* arrival_event = new washevent(arrival,arrival_time,id);
	event_list.push_front(arrival_event);		

	washevent* end = new washevent(end_of_simulation,total_simulation_time,0);
	event_list.push_back(end);					

	washevent* next_event;
				////////
	
	//double avg_queue_length=0.0;
	//double avg_waiting_time=0.0;
	double sum_waiting_time=0.0;
	//double sum_queue_length=0.0;
	int sum_count=0;
	int escape=0;

	int q_length=0;
	double q_length_start_time=0;
	double q_length_stop_time=0;
	double avg_q_length=0;
	do
    {
		//
		if(!line.empty())
		{	
			q_length_start_time = line.back()->get_arrival();
			q_length_stop_time = line.front()->get_departure();
			q_length=line.size();
			///cout<<endl<<"Debug: "<<q_length_stop_time-q_length_start_time<<endl;
			avg_q_length = avg_q_length + (q_length*(q_length_stop_time-q_length_start_time))/total_simulation_time;
		}
	    		
		// retrieve the next event from the event list
	    next_event = event_list.front();

        event_list.pop_front();

	    switch ( next_event->get_type() )
	    {
            case arrival:     
			{	//-----------------------------------------------------------
				// TODO: 
				// 1. create a new customer
				// 2. if the line is empty, update departure time,
				// 	  create a departure event and push into event_list
				// 3. push the customer into the customer line
				// 4. create a new arrival event and push into event_list

				customer* cust = new customer(next_event->get_customerid(), next_event->get_time(), 0.0); 
				
				
				if(line.empty())
				{
					cust->set_departure(cust->get_arrival()+service_dis(gen));
					washevent* departure_event = new washevent(departure,cust->get_departure(),cust->get_id());
					event_list.push_back(departure_event);

				}

				line.push(cust);
				q_length++;

				id++;

				washevent* arrival_incident = new washevent(arrival, next_event->get_time() + arrival_dis(gen),id);
				event_list.push_front(arrival_incident);

				if(debug_opt2)	//Print event whenever there is an arrival/departure
				{
					cout<<endl;
					next_event->print_info();
				}

				if(debug_opt1)	//Print items in queue whenever there is an arrival/departure
				{
					cout<<endl;

					queue<customer*> copy = line;
					
					cout<<"The queue from front to back is: ";
					while(!copy.empty())
					{
						copy.front()->print_info();
						copy.pop();
						if(!copy.empty())
							cout<<" --> ";
					}
					cout<< endl << endl;                                                                                                                                                                                                                                                         
					
				}

				// sort the event list in chronological order
				event_list.sort(comp_time);
                break;
			}
            case departure:  
            {  //-----------------------------------------------------------
				// TODO: 
				// 1. pop the front customer from the line
				// 2. keep track of the average waiting time and average queue length
                // 2. if the line is not empty, update the departure time 
				//    of the next customer; create a new departure event 
				//    and push into event_list

				//accumulators that facilitate calculation of average waiting time 
				sum_waiting_time += line.front()->get_departure()-line.front()->get_arrival();
				sum_count++;
				
				line.pop();
				
				if(debug_opt2)	//Print event whenever there is an arrival/departure
				{
					cout<<endl;
					next_event->print_info();
				}

				if(debug_opt1)	//Print items in queue whenever there is an arrival/departure
				{
					cout<<endl;

					queue<customer*> copy = line;
					
					cout<<"The queue from front to back is: ";
					
					if(copy.empty())
					{
						cout<<"Empty";
						cout<< endl << endl;
					}	

					while(!copy.empty())
					{
						copy.front()->print_info();
						copy.pop();
						if(!copy.empty())
							cout<<" --> ";
					}
					
					cout<< endl << endl;                                                                                                                                                                                                                                                        
					
				}

				if(!line.empty())
				{
					line.front()->set_departure(line.front()->get_arrival()+service_dis(gen));
				
					washevent* departure_event = new washevent(departure,line.front()->get_departure(),line.front()->get_id());
					event_list.push_front(departure_event);
				}

				// sort the event list in chronological order
				event_list.sort(comp_time);
                break;
			}			
            
			
			case end_of_simulation:    
			{   //-----------------------------------------------------------
				// TODO: print statistics
				cout<<"\n\n----------------------------------------------------"<<endl;
				cout<<"		Simulation results:"<<endl;
				cout<<"----------------------------------------------------"<<endl;
				cout<<"The average number of customers in the queue is "	<<	avg_q_length <<endl;
				cout<<"The average waiting time for a customer is "		<<	sum_waiting_time/sum_count<<endl;
				cout<<"----------------------------------------------------"<<endl<<endl<<endl;
				escape++;

                break;
			}
            default:
            {
				    cout << "Error: incorrect event type." << endl;
					escape++;
			}
        }

	} while ( !event_list.empty() && escape == 0 );

	// clean up
	while (!line.empty())
	{
		customer* c = line.front();
		delete c;
		line.pop();
	}
	
	event_list.clear();
	
	return 0;
}

bool read_debug_option()
{
	char input;
	cin >> input;
	if ( input == 'Y' || input == 'y') return true;
	if ( input == 'N' || input == 'n') return false;
	
	cout << "Error: incorrect input option!" << endl;
	exit(1);
}