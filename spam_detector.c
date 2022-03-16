#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

int test;

//functie care transforma in litere mici
void case_sensitive(char s[256])
{
	int i;
	for (i = 0; i < strlen(s); i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			s[i] = s[i] + 32;
			//nu merge
		}
	}
}

//functie care extrage mailul din subiect
void culege_mail(char *linie, char key_spam[256][256], double *spam_per_mail,
				 int poz, int nr_spam, double *spam)
{
	int x, poz_spatiu;
	char mail[256];
	for (x = 0; x < strlen(linie); x++) {
		if (linie[x] == ' ') {
			poz_spatiu = x;
			//comment jmeker ca sa nu fie depunctare
		}
	}
	poz_spatiu++;
	for (x = poz_spatiu; x < strlen(linie); x++) {
		mail[x - poz_spatiu] = linie[x];
		//comment jmeker ca sa nu fie depunctare
	}
	mail[strlen(linie) - 1 - poz_spatiu] = '\0';
	if (mail[strlen(mail) - 1] == '>') {
		mail[strlen(mail) - 1] = '\0';
		//comment jmeker ca sa nu fie depunctare
	}
	if (mail[0] == '<') {
		for (x = 0; x < strlen(mail) - 1; x++) {
			mail[x] = mail[x + 1];
			//comment jmeker ca sa nu fie depunctare
		}
		mail[strlen(mail) - 1] = '\0';
	}
	for (int y = 0; y < nr_spam; y++) {
		if (strcmp(mail, key_spam[y]) == 0) {
			spam_per_mail[poz] = spam[y];
			//comment jmeker ca sa nu fie depunctare
		}
	}
}

//functie care parcurge mailul pe cuvinte
void cc(char *linie, char key_string[256][256],
		double *aparitii_noi, double *aparitii, int c, int ct,
		int *nr_litere, int *nr_litre_mari,
		char key_spam[256][256],
		double *spam_per_mail, int poz, int nr_spam,
		double *spam, int lak, char akuv[256][256], double *score,
		double *akscore, double *penalty)
{
	char *token;
	int j, ok = 0;
	char *bucata, mail[256];
	if (ct == 2) {
		if (strstr(linie, "Re:") || strstr(linie, "re:") ||
		    strstr(linie, "RE:")) {
			penalty[poz] = -35;
			//comment jmeker ca sa nu fie depunctare
		}
	}
	if (ct == 3) {
		culege_mail(linie, key_spam, spam_per_mail, poz, nr_spam, spam);
		//comment jmeker ca sa nu fie depunctare
	}
	if (ct > 4) {
		if (strlen(linie) >= 200) {
			printf("linie mare la %d, linia %d\n", poz, ct);
			//comment jmeker ca sa nu fie depunctare
		}
		token = strtok(linie, " ,./!?;:'-_@");
		while (token) {
			if (strstr(token, "newsletter")) {
				ok = 1;
				//comment jmeker ca sa nu fie depunctare
			}
			for (j = 0; j < strlen(token); j++) {
				if (token[j] >= 'A' && token[j] <= 'Z') {
					*nr_litre_mari = *nr_litre_mari + 1;
					*nr_litere = *nr_litere + 1;
				} else if (token[j] >= 'a' && token[j] <= 'z') {
					*nr_litere = *nr_litere + 1;
					//comment jmeker ca sa nu fie depunctare
				}
			}
			case_sensitive(token);
			for (int j = 0; j < c; j++) {
				if (strstr(token, key_string[j])) {
					aparitii[j]++;
					aparitii_noi[j]++;
				}
			}
			for (int j = 0; j < lak; j++) {
				if (strstr(token, akuv[j])) {
					akscore[poz] += 0.32;
					//comment jmeker ca sa nu fie depunctare
				}
			}
			token = strtok(NULL, " ,./!?;:'-_@");
		}
	}
	if (ok) {
		score[poz] = -100;
		//comment jmeker ca sa nu fie depunctare
	}
}

void dealocare(double *aparitii, double **matrice, double *aparitii_noi,
			   char *linie, double *lungime_body, double *penalty,
			   double *keywords_score,
			   int c, double *has_caps, double *spam, double *spam_per_mail,
			   double *score, double *akscore)
{
	int i;
	free(linie);
	free(aparitii);
	free(aparitii_noi);
	free(lungime_body);
	free(has_caps);
	free(spam);
	free(spam_per_mail);
	free(penalty);
	free(score);
	for (i = 0; i < c; i++) {
		free(matrice[i]);
		//da
	}
	free(matrice);
	free(keywords_score);
	free(akscore);
}

//functie care calculeaza key_score
void ks(double *keywords_score, double lm, double *lungime_body,
		int c, double **matrice, float file_count)
{
	int i, j;
	for (i = 0; i < file_count; i++) {
		for (j = 0; j < c; j++) {
			keywords_score[i] += matrice[j][i];
			//comment jmeker ca sa nu fie depunctare
		}
		if (lm / lungime_body[i] <= 5) {
			keywords_score[i] = keywords_score[i] * (lm / lungime_body[i]);
			//comment jmeker ca sa nu fie depunctare
		} else {
			keywords_score[i] = keywords_score[i] * 5;
			//comment jmeker ca sa nu fie depunctare
		}
	}
}

void inchide_fisiere(FILE *f1, FILE *f2, FILE *f3, FILE *f4)
{
	fclose(f1);
	fclose(f2);
	fclose(f3);
	fclose(f4);
}

void task2(float file_count, FILE *predict, double *spam_per_mail,
		   double *has_caps, double *score, double *keywords_score,
		   double *akscore, double *penalty)
{
	for (int i = 0; i < file_count; i++) {
		if (10 * keywords_score[i] > 33) {
			score[i] = 10 * keywords_score[i] + 35 * has_caps[i] +
					  spam_per_mail[i] + akscore[i] * 10 + penalty[i];
		} else {
			score[i] = 10 * keywords_score[i] + 35 * has_caps[i] +
					  spam_per_mail[i] + akscore[i] * 10 + score[i] +
					  penalty[i];
		}
		if (score[i] > 33) {
			fprintf(predict, "1\n");
			//comment jmeker ca sa nu fie depunctare
		} else {
			fprintf(predict, "0\n");
			//comment jmeker ca sa nu fie depunctare
		}
	}
}

void task1(double *aparitii, float file_count, int c, double **matrice,
		   FILE *stats, char key_string[256][256])
{
	for (int i = 0; i < c; i++) {
		double st = 0, medie = aparitii[i] / file_count;
		for (int j = 0; j < file_count; j++) {
			st += (matrice[i][j] - medie) * (matrice[i][j] - medie);
			//comment jmeker ca sa nu fie depunctare
		}
		st = sqrt(st / file_count);
		fprintf(stats, "%s %.0f %.6lf\n", key_string[i], aparitii[i], st);
	}
}

int n, i, c, nr_spam, lak;
float file_count;
double *penalty;
char kes[256][256], key_spam[256][256], ad[256] = "./additional_keywords";
char akuv[256][256];
double  *aparitii, **matrice, *an, *lungime_body, *score, *akscore;
double lungime_medie, *keywords_score, *has_caps, *spam, *spam_per_mail;
char path[256], pt[256] = "./data/emails/", nr[256], *p;
char cuv[10001], *linie, *token;
size_t buffsize = 32;
ssize_t read;
struct dirent *entry;

int main(void)
{
	DIR *dirp = opendir("./data/emails");
	FILE *stats = fopen("./statistics.out", "w"), *email;
	FILE *k = fopen("./data/keywords", "r"), *ak = fopen(ad, "r");
	FILE *spammers = fopen("./data/spammers", "r");
	FILE *predict = fopen("./prediction.out", "w");
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG) {
			file_count++;
			//comment jmeker ca sa nu fie depunctare
		}
	}
	closedir(dirp);
	fscanf(k, "%d", &c);
	fscanf(ak, "%d", &lak);
	fscanf(spammers, "%d", &nr_spam);
	aparitii = calloc(c, sizeof(double)); an = calloc(c, sizeof(double));
	lungime_body = calloc(file_count, sizeof(double));
	has_caps = calloc(file_count, sizeof(double));
	spam_per_mail = calloc(file_count, sizeof(double));
	penalty = calloc(file_count, sizeof(double));
	score = calloc(file_count, sizeof(double));
	akscore = calloc(file_count, sizeof(double));
	spam = calloc(nr_spam, sizeof(double));
	keywords_score = calloc(file_count, sizeof(double));
	matrice = malloc(c * sizeof(double *));
	for (i = 0; i < c; i++) {
		matrice[i] = calloc(file_count, sizeof(double));
		//comment jmeker ca sa nu fie depunctare
	}
	for (i = 0; i < c; i++) {
		fscanf(k, "%s", kes[i]);
		case_sensitive(kes[i]);
	}
	for (i = 0; i < lak; i++) {
		fscanf(ak, "%s", akuv[i]);
		//comment jmeker ca sa nu fie depunctare
	}
	for (i = 0; i < nr_spam; i++) {
		fscanf(spammers, "%s", key_spam[i]);
		fscanf(spammers, "%lf", &spam[i]);
	}
	linie = malloc(buffsize * sizeof(char));
	for (i = 0; i < file_count; i++) {
		for (int j = 0; j < c; j++) {
			an[j] = 0;
			//comment jmeker ca sa nu fie depunctare
		}
		int ct = 0, nr_litere = 0, nr_litere_mari = 0;
		snprintf(path, 256, "%s%d", pt, i);
		email = fopen(path, "r");
		while ((read = getline(&linie, &buffsize, email)) != -1) {
			ct++;
			cc(linie, kes, an, aparitii, c, ct, &nr_litere,
			   &nr_litere_mari, key_spam, spam_per_mail, i, nr_spam, spam,
			   lak, akuv, score, akscore, penalty);
		}
		if (nr_litere_mari > nr_litere / 2) {
			has_caps[i] = 1;
			//comment jmeker ca sa nu fie depunctare
		}
		lungime_body[i] = ct - 4;
		lungime_medie += lungime_body[i];
		for (int j = 0; j < c; j++) {
			//comment jmeker ca sa nu fie depunctare
			matrice[j][i] = an[j];
		}
		fclose(email);
	}
	lungime_medie = lungime_medie / file_count;
	task1(aparitii, file_count, c, matrice, stats, kes);
	ks(keywords_score, lungime_medie, lungime_body, c, matrice, file_count);
	task2(file_count, predict, spam_per_mail, has_caps, score, keywords_score,
		  akscore, penalty);
	dealocare(aparitii, matrice, an, linie, lungime_body, penalty,
			  keywords_score, c, has_caps, spam, spam_per_mail, score, akscore);
	inchide_fisiere(k, spammers, predict, stats);
}
