#include "bow.h"

int main() {
   struct bag_struct *ham_bag;
   struct bag_struct *spam_bag;
   /* initialize both the ham and the spam bag of words */
   ham_bag = new_bag();
   spam_bag = new_bag();
   /* read both the ham and the spam labels */
   read_sms_data(ham_bag,"ham");
   read_sms_data(spam_bag,"spam");
   /* calculate the ratios and display them */
   differences(ham_bag,spam_bag);
   /* free the memory */
   free(ham_bag);
   free(spam_bag);
   return 0;
}

