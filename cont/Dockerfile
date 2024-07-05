FROM fedora:39
 
RUN dnf -y update
RUN dnf install -y make redhat-rpm-config
 
RUN dnf install -y https://download.postgresql.org/pub/repos/yum/reporpms/F-39-x86_64/pgdg-fedora-repo-latest.noarch.rpm
 
RUN dnf install -y postgresql16-devel postgresql16-server postgresql16-contrib postgresql16
 
COPY PostgreSQL-Core-C   /usr/local/src/PostgresSQL-Core-C
 
#RUN export PATH=$PATH:/usr/pgsql-16/bin/
 
#ENV PATH="$PATH:/usr/pgsql-16/bin/"
 
#RUN echo $PATH
# Build and install your extension
#RUN cd /usr/local/src/PostgresSQL-Core-C  && make && make install
 
 
#USER postgres
 
#ENV PATH="$PATH:/usr/pgsql-16/bin/"
 
#RUN echo $PATH
 
#ENV PGDATA=/var/lib/pgsql/16/data/
#USER root
 
#RUN cd /usr/local/src/PostgresSQL-Core-C  && make && make install
 
#RUN /usr/pgsql-16/bin/pg_ctl initdb
#RUN /usr/pgsql-16/bin/pg_ctl -D /var/lib/pgsql/16/data/ start
 
#USER root
#ENV PATH="$PATH:/usr/pgsql-16/bin/"
#RUN cd /usr/local/src/PostgresSQL-Core-C  && make && make install
 
 
#CMD ["/usr/pgsql-16/bin/pg_ctl", "initdb"]
#CMD ["/usr/pgsql-16/bin/pg_ctl", "-D", "/var/lib/pgsql/16/data/", "start"]
