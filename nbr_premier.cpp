#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
mutex locker;
std::vector<bool> TableauCrible;
std::vector<int> TableauBrute;
// fonction qui determine si un nombre premier ou pas
bool nbr_premier(long n)
{	if(n==0 || n==1)
		return false;
	for (auto i = 2; i <= sqrt(n) ; ++i)
		if(n%i==0)
			return false;
	return true;	
}

/*
	Fonction qui pour debut jusqua fin determine si un nombre est premier ou pas et si oui l'insere dans 
	TableauBrute
*/
void BruteForce1(long debut,long fin){
for (long i = debut; i < fin ; ++i)
{	if(nbr_premier(i))
		{		
			locker.lock();
			TableauBrute.push_back(i);	
			locker.unlock();

		} 
	}
}


/*
	fonction qui de depart a fin determine si un nombre est premier ou pas et qui 
	s'incremente selon le nombre de thread
*/
void BruteForce2(int nbr_thread,int depart,int fin)
{
	for (int i = depart; i < fin; i+=nbr_thread)
	{
		if(nbr_premier(i))
		{		
			locker.lock();
			TableauBrute.push_back(i);	
			locker.unlock();

		} 


	}
}
/*
fonction qui de debut a fin determine si un nombre une case du tableau est premiere ou pas ;
la case du tableau represente la valeur si elle est vraie elle est premier sinon elle ne l'est pas
*/
void cribleUn(long debut,long fin)
{	
	for (long i = debut; i < fin; i++)
	{	
		if(TableauCrible[i]!=false)
			for (size_t j = i*i; j < TableauCrible.size(); j+=i)			
				TableauCrible[j]=false;		
	}
}

/*
	ndlr comme Bruteforce2
*/
void CribleDeux(int debut,int fin,int nbr_thread)
{	if(debut>=fin)
		return;	
	if(TableauCrible[debut]==false)
		{
			CribleDeux(debut+nbr_thread,nbr_thread,fin);
		}
	for (int i = debut; i < fin; ++i)
	{
		if(TableauCrible[i]==true)
			TableauCrible[i]=false;
	}
	CribleDeux(debut+nbr_thread,nbr_thread,fin);

}
/*
	fonction qui initilase le tableau de vecteur a true
*/
void init()
{	TableauCrible[0]=false;
	TableauCrible[1]=false;
	for (size_t i = 2; i < TableauCrible.size(); ++i)
	{
		TableauCrible[i]=true;
	}
}
/*
	fonction qui affiche les nombre premier du tableau de crible ; ceux resté a true
*/
void affiche()
{
	for (size_t i = 0; i < TableauCrible.size(); ++i)
	{
		if(TableauCrible[i]==true)
			cout<<i<<endl;
		
	}
}
/*

	cette fonction est utilise pour ecrire le crible dans un fichier
*/
void DeVectAFile(string name)
{  	   ofstream monFlux(name);
		string ecrire;
	   for (size_t i = 0; i < TableauCrible.size(); ++i)
	   {	if(TableauCrible[i]!=false)
	 		  	ecrire
	 		  	+=to_string(i)+"\n";
		   }
		  monFlux<<ecrire; 
	   monFlux.close();

}

/*

	cette fonction est utilise pour ecrire le bruteforce dans un fichier
*/

void DeVectAFileDeux(string name,long debut,long fin){
	   ofstream monFlux(name);
	   string ecrire;

	   for (auto i = debut; i < fin; ++i)
	   	ecrire+=to_string(TableauBrute[i])+"\n";
	   monFlux<<ecrire;
	   monFlux.close();
}

int main(int argc, char const *argv[])
{	/*
	verifie que les parametre envoyé sont correct
		*/
	if(!argv[1] || !argv[2] || atoi(argv[1])<=0  || atoi(argv[2])<=0){
		fprintf(stderr, "%s\n", "erreur de parametre <N a attendre> <Thread>");
		exit(0);
		}
	//suite a une marge d'erreur sur le calcul plus bas je prefere
	//perdre legerement en seconde ~5 ms plutot que perdre en valeur
	//pour les valeurs	
	int nbr_thread=atoi(argv[2]),nbr_attendre=atoi(argv[1]);
	std::vector<thread> tab(nbr_thread);
	TableauCrible.resize(nbr_attendre);
	char fichier[100];
	// en ecrit dans le fichiers sous la forme nombre a atteindre ensuite thread 
	sprintf(fichier,"Test/MethodeCrable_NonThread:%d_%d.txt",nbr_attendre,nbr_thread);	
   /*
	j'utilse 5 methode : crible sans thread
						crible avec thread
						brute force threade avec une repartition des valeurs egale
						brute force avec une repartition des valeurs en fonction racine 
						brute force qui s'incremente du nombre de thread dispo
   */
    auto avant = std::chrono::system_clock::now();
    init();
	cribleUn(2,TableauCrible.size());	
    DeVectAFile(fichier);
	TableauCrible.clear();
	init();
	sprintf(fichier,"Test/MethodeCrible_Thread:%d_%d.txt",nbr_attendre,nbr_thread);	
    auto apres = std::chrono::system_clock::now();
    auto duree = apres - avant;
    std::cout << "methode crable sans thread : "<< std::chrono::duration_cast<std::chrono::milliseconds>(duree).count() << "ms" << '\n';
 	avant = std::chrono::system_clock::now();
	for (auto i = 0; i <nbr_thread; ++i)
	{
		tab[i]=thread(CribleDeux,i+2,nbr_attendre,nbr_thread);
	}
	for (long i = 0; i < nbr_thread; ++i)
	{
		tab[i].join();
	}
	/*
		j'ecris aussi le fichiers en non parallele , plus bas il y'as en mode parralle
	*/
    std::cout << "methode crable avec thread : "<< std::chrono::duration_cast<std::chrono::milliseconds>(duree).count() << "ms" << '\n';
	apres = std::chrono::system_clock::now();
    duree = apres - avant ;
    avant = std::chrono::system_clock::now();
	for (auto i = 0; i <nbr_thread; ++i)
	{
		tab[i]=thread(BruteForce1,nbr_attendre*(sqrt(i*5)/sqrt(nbr_thread*5)) , nbr_attendre*(sqrt((i+1)*5)/sqrt(nbr_thread*5) ));
	}
	for (long i = 0; i < nbr_thread; ++i)
	{
		tab[i].join();
	}
	TableauBrute.clear();
	apres = std::chrono::system_clock::now();
    duree = apres - avant ;
    std::cout<< "methode brute Un repartition sqrt: "<<std::chrono::duration_cast<std::chrono::milliseconds>(duree).count() << "ms" << '\n';
  
	avant = std::chrono::system_clock::now();
	for (auto i = 0; i <nbr_thread; ++i)
	{
		tab[i]=thread(BruteForce1,(nbr_attendre*i)/nbr_thread,(nbr_attendre*(i+1))/nbr_thread);
	}
	for (long i = 0; i < nbr_thread; ++i)
		{
		tab[i].join();
	}

	
	TableauBrute.clear();
	apres = std::chrono::system_clock::now();
    duree = apres - avant ;
    std::cout<< "methode brute Deux repartition standard:  "<<std::chrono::duration_cast<std::chrono::milliseconds>(duree).count() << "ms" << '\n';
	avant = std::chrono::system_clock::now();
	for (auto i = 0; i <nbr_thread; ++i)
	{
		tab[i]=thread(BruteForce2,nbr_thread,i,nbr_attendre);
	}
	for (long i = 0; i < nbr_thread; ++i)
	{
		tab[i].join();
	}

//	TableauBrute.clear();
	
	apres = std::chrono::system_clock::now();
    duree = apres - avant ;
    std::cout<< "methode brute Trois thread avanceé :  "<<std::chrono::duration_cast<std::chrono::milliseconds>(duree).count() << "ms" << '\n';

	
	//	partie dans le but de parallelisze l'ecriture des fichier
	
 	sort(TableauBrute.begin(),TableauBrute.end());
 	/*
		je parallelize aussi l'ecriture de fichier dans plusieurs fichiers nomme selon le thread + 1 qui s"on occupe occupe
 	*/
	for (int i = 0; i < nbr_thread; ++i)
	{	
		sprintf(fichier,"Test/N_%d_threadN_%d_nbrthread_%d.txt",nbr_attendre,i+1,nbr_thread);
		tab[i]=thread(DeVectAFileDeux,fichier,(i*TableauBrute.size())/nbr_thread , ((i+1)*TableauBrute.size())/nbr_thread);
	}	


	for (int i = 0; i < nbr_thread; ++i)
		{	
			tab[i].join();

		}

	
	return 0;
}