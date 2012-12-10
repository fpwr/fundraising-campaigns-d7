DIRS="civi_cache dirtied_cache contact_attempts failed failed_invoices host_submit PDF_Receipts Receipt_Cache"
mkdir $DIRS
chown apache.apache $DIRS
chmod 755 $DIRS
